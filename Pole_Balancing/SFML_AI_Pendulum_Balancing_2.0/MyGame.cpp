#include "MyGame.h"

MyGame::MyGame()
    : image(nullptr), AI_Active(sf::Vector2f(0, 50 * SH)),
      time(sf::Vector2f(0, 200 * SH)), generation(sf::Vector2f(0, 100 * SH)),
      population(sf::Vector2f(0, 150 * SH)), fitness(sf::Vector2f(0, 250 * SH))
{
    this->pop = nullptr;
}

void MyGame::updatePopulation(const Population<NEAT::Network> *pop, unsigned n)
{
    this->pop = pop;
    N = n;
    generation.setString("Generation: " + std::to_string(++generation_c));
}

// called once before the game starts
bool MyGame::onCreate()
{
    AI_Active.setString(
        game_state == TRAIN_AI
            ? "Evolution"
            : (game_state == BEST_AI
                   ? "Best AI"
                   : (game_state == DOUBLE ? "Player - Double"
                                           : "Player - Single")));

    player_double = new UserDoublePendulum(&window);
    player_single = new UserPendulum(&window);
    // BestAI = new Pendulum(&window,
    // "../../../GAcpp/NEAT/v2/PoleBalancing_360/pole_best.txt");
    BestAI = new Pendulum(&window, "pole_best.txt");
    BestAI->setAllowUserInput(true);

    for (auto &pendulum : pendulums)
        delete pendulum;
    pendulums.clear();

    unsigned tot = 0;
    for (unsigned i = 0; i < pop->species.size(); ++i)
        tot += pop->species[i]->pop.size();

    unsigned n = std::min(N, tot);

    unsigned iter = 0;
    while (n > pop->species.size())
    {
        for (unsigned i = 0; i < pop->species.size(); ++i)
        {
            if (pop->species[i]->bestRank == iter)
                continue;
            if (pop->species[i]->pop.size() <= iter)
                continue;
            pendulums.push_back(new Pendulum(&window,
                                             pop->species[i]->pop[iter]->I,
                                             sf::Color(255, 255, 255, 150)));
            --n;
        }
        ++iter;
    }

    for (unsigned i = 0; i < pop->species.size() && n > 1; ++i)
    {
        if (pop->bestSpecies != i)
            pendulums.push_back(new Pendulum(&window,
                                             pop->species[i]->getBest(),
                                             sf::Color(0, 0, 255, 150)));
        --n;
    }

    pendulums.push_back(
        new Pendulum(&window, pop->getBest(), sf::Color(0, 255, 0, 100)));

    std::cout << pop->species[pop->bestSpecies]
                     ->pop[pop->species[pop->bestSpecies]->bestRank]
                     ->fitness_orig
              << std::endl;

    const NEAT::Network *best = &pop->getBest();
    image = new NEAT::Image(*best, "params_image.txt");
    image->evolve();
    // image->draw(300. * SW, 200. * SH);
    network_im = image->makeImage(400 * SW, 400 * SH);
    network_im.setPosition(SCREENWIDTH - 420 * SW, 420 * SH);

    return true;
}

// first thing to be called every frame
bool MyGame::onHandleEvent(GF::Event &event)
{
    if (getFPS() < getMaxFPS() * 0.9)
        return true;

    if (game_state == TRAIN_AI)
        image->handleEvent();

    static GF::ToggleKey E(sf::Keyboard::E);
    if (E.isKeyPressed())
    {
        window.close();
        return false;
    }

    static GF::ToggleKey A(sf::Keyboard::A);
    if (A.isKeyPressedOnce(event))
    {
        game_state = TRAIN_AI;
        AI_Active.setString("Evolution");
    }

    static GF::ToggleKey B(sf::Keyboard::B);
    if (B.isKeyPressedOnce(event))
    {
        game_state = BEST_AI;
        AI_Active.setString("Best AI");
    }

    static GF::ToggleKey D(sf::Keyboard::D);
    if (D.isKeyPressedOnce(event))
    {
        game_state = DOUBLE;
        delete player_double;
        player_double = new UserDoublePendulum(&window);
        AI_Active.setString("Player _ Double");
    }

    static GF::ToggleKey S(sf::Keyboard::S);
    if (S.isKeyPressedOnce(event))
    {
        game_state = SINGLE;
        delete player_single;
        player_single = new UserPendulum(&window);
        AI_Active.setString("Player _ Single");
    }

    if (game_state == TRAIN_AI)
    {
        for (auto &pendulum : pendulums)
            if (!pendulum->isdead)
                pendulum->handleEvent(event);
    }

    if (game_state == SINGLE)
        player_single->handleEvent(event);
    if (game_state == DOUBLE)
        player_double->handleEvent(event);
    if (game_state == BEST_AI)
        BestAI->handleEvent(event);

    return true;
};

// called every frame before draw
bool MyGame::onUpdate(const float fElapsedTime, const float fTotalTime)
{
    if (getFPS() < getMaxFPS() * 0.9)
        return true;

    if (game_state == TRAIN_AI)
    {

        bool out = false;
        for (auto &pendulum : pendulums)
        {
            out |= pendulum->update(fElapsedTime, fTotalTime);
        }

        time.setString("Time: " + std::to_string(fTotalTime * SIM_SPEED));
        if (!out)
            image->close();
        return out;
    }

    if (game_state == SINGLE)
        player_single->update(fElapsedTime, fTotalTime);

    if (game_state == DOUBLE)
        player_double->update(fElapsedTime, fTotalTime);

    if (game_state == BEST_AI)
        return BestAI->update(fElapsedTime, fTotalTime);

    return true;
};

// last thing to be called every frame
bool MyGame::onDraw()
{
    if (game_state == TRAIN_AI)
    {
        unsigned population_c = 0;
        for (auto &pendulum : pendulums)
        {
            if (!pendulum->isdead)
            {
                ++population_c;
                pendulum->draw();
            }
        }
        population.setString("Population size: " +
                             std::to_string(population_c));
        fitness.setString("Best fitness : " +
                          std::to_string(pop->getBestFitness()));

        window.draw(time);
        window.draw(generation);
        window.draw(population);
        window.draw(fitness);

        window.draw(network_im);
    }

    if (game_state == SINGLE)
        player_single->draw();

    if (game_state == DOUBLE)
        player_double->draw();

    if (game_state == BEST_AI)
        BestAI->draw();

    window.draw(GF::Line(sf::Vector2f(0, SCREENHEIGHT - 100 * SH),
                         sf::Vector2f(SCREENWIDTH, SCREENHEIGHT - 100 * SH)));

    window.draw(AI_Active);

    GF::Text text(
        "E - Exit 	A - Evolution 	B - Best AI 	D - Player _ Double "
        "	S - Player _ Single 	SPACE - Restart player 	ESC - Skip "
        "Generation (Evolution) 	(ESC + P) - Skip Generation fast",
        TOPLEFT_F + sf::Vector2f(650 * SW, 30 * SH), 15 * SW);
    window.draw(text);
    window.draw(network_im);

    return true;
};

void MyGame::onDestroy()
{
    for (auto &pendulum : pendulums)
        delete pendulum;
    pendulums.clear();

    delete player_double;
    delete player_single;
    delete BestAI;

    if (image)
    {
        delete image;
        image = nullptr;
    }

}
