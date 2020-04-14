#include "SBestReplay.h"

#include "unistd.h"

SBestReplay::SBestReplay(GF::StateMachine *sm, sf::RenderWindow *r)
    : ga("params.txt"), image(nullptr), started(false)
{
    renderer = r;
    stateMachine = sm;
}

bool alive_condition(const GameEntities &AI, float elapsedTime)
{
    return AI.player.getActive() && elapsedTime < BEST_FITNESS;
}

VecD makeAIinput(GameEntities &enti)
{
    unsigned howmany = 1;
    auto closest = enti.player.closestBots(enti.bots, howmany);
    // if(closest.size()) std::cout << "Closest Distance = " << closest[0].first
    // << std::endl; std::cout << "Player Health = " << enti.player.getHealth()
    // << "\t(" << enti.player.getX() << "," << enti.player.getY() << ")" <<
    // std::endl;

    static float minX = boundary_x;
    static float minY = boundary_y;
    static float maxX = SCREENWIDTH - boundary_x;
    static float maxY = SCREENHEIGHT - boundary_x;
    static float distanceMax = std::max(SCREENWIDTH, SCREENHEIGHT);

    VecD inputs(2 + howmany * 2);
    inputs[0] = (enti.player.getX() - minX) / (maxX - minX);
    inputs[1] = (enti.player.getY() - minY) / (maxY - minY);

    unsigned idx_min = std::min(howmany, (unsigned)closest.size());

    for (unsigned i = 0; i < idx_min; ++i)
    {
        inputs[2 + 2 * i] = closest[i].first / distanceMax;
        inputs[2 + 2 * i + 1] = closest[i].second / (2. * M_PI);
    }

    for (unsigned i = idx_min; i < howmany; ++i)
    {
        inputs[2 + 2 * i] = distanceMax * 2. / distanceMax;
        inputs[2 + 2 * i + 1] = 0.;
    }

    return inputs;
}

std::pair<float, bool> SBestReplay::stepAI(GameEntities &AI,
                                           const NEAT::Network &net,
                                           float fElapsedTime)
{
    VecD inputs = makeAIinput(AI);

    VecD outputs = net.evaluate(inputs);
    // VecD outputs(4);

    float moveX = (2. * outputs[0] - 1.);
    float moveY = (2. * outputs[1] - 1.);
    bool shootPressed = (outputs[2] > 0.5);
    float mouseAngle = 2. * M_PI * outputs[3];
    float shootX = AI.player.getX() + cos(mouseAngle);
    float shootY = AI.player.getY() + sin(mouseAngle);
    /*
        float moveX = 0.;
        float moveY = 0.;
        bool shootPressed = 1;
        float mouseAngle = 2. * M_PI * inputs[3];
        float shootX = AI.player.getX() + cos(mouseAngle);
        float shootY = AI.player.getY() + sin(mouseAngle);
    */

    auto buttons = SurvivalGame::getButtons();
    int min_price = AI.player.getMoney();
    int chosenButton = -1;
    if (min_price > buttons[0].getPrice())
        chosenButton = 0;
    if (min_price > buttons[1].getPrice())
        chosenButton = 1;
    if (min_price > buttons[2].getPrice())
        chosenButton = 2;
    if (min_price > buttons[3].getPrice())
        chosenButton = 3;

    bool shotshot = SBestReplay::step(AI, shootPressed, shootX, shootY, moveX,
                                      moveY, chosenButton, fElapsedTime);
    // step(AI, ShootPressed, ShootAtX, ShootAtY, HorizontalMovement,
    // VerticlaMovement, ChosenUpgrade, ElapsedTime);

    return std::pair<float, bool>(mouseAngle, shotshot);
}

// calculate the fitness of the given network
double fitnessFunc(const NEAT::Network &net,
                   const GA::Evolver<NEAT::Network> *ga)
{
    // int numberUpgrades = ((SurvivalEvolver*)ga)->maxUpgrades;
    /*    GameEntities &AI = ((SurvivalEvolver *)ga)->AI;
        AI.bot_number = 0;
        AI.botPeriod = 0.;
        AI.shotsPeriod = 0.;
        AI.botDamagePeriod = 0.;
        AI.player.respawn();
        AI.player.setUp(SW * 300., SH * 300.);
        for (int i = 0; i < max_bots; ++i)
            AI.bots[i].restart();*/
    GameEntities AI;
    AI.player.setUp(SW * 300., SH * 300.);

    float elapsedTime = 0.;
    static float dt = 1. / 60.;

    while (alive_condition(AI, elapsedTime))
    {
        elapsedTime += dt;
        SBestReplay::stepAI(AI, net, dt);
    }

    return elapsedTime;
    // return 0;
}

void SBestReplay::newGeneration(bool skip_unchanged, unsigned skips)
{
    double lastFitness = ga.getBestFitness();
    unsigned count_skips = 0;

    // starts a new generation, or skips multiples if skip_unchanged == true
    if (!started)
    {
        ga.start();
        started = true;
    }
    else
    {
        // do at least one step. Do more steps if skip_unchanged == true ( do
        // 'skips' number of steps)
        do
        {
            ga.step();

        } while (skip_unchanged && lastFitness == ga.getBestFitness() &&
                 (++count_skips < skips || skips == 0)

                 && ga.getBestFitness() < BEST_FITNESS);
    }

    if (image)
        delete image;

    // find best image configuration of the best individual's neural network
    image = new NEAT::Image(ga.getBest(), "params_image.txt");
    image->evolve();

    // setup a drawable image of the neural network to display
    network_im = image->makeImage(200 * SW, 200 * SH);
    network_im.setPosition(SCREENWIDTH - 220 * SW, 200 * SH);

    best = &ga.getPopulation()->getBest();
    game.player.setUp(SW * 300., SH * 300.);
    game.player.respawn();

    for (unsigned i = 0; i < max_bots; ++i)
    {
        game.bots[i].restart();
    }

    Bot::setPeriodD(Bot::getPeriod0());

    Shot::setDamageD(Shot::getDamage0());
    Bot::setDamageD(Bot::getDamage0());

    Shot::setHealthD(Shot::getHealth0());
    Bot::setHealthD(Bot::getHealth0());
}

// called once before the loop starts
bool SBestReplay::onCreate()
{
    ga.addCriteria(GA::StopReason::BestMaximum, {BEST_FITNESS});
    ga.setEvaluate(fitnessFunc, GA::MAXIMIZE);
    ga.maxUpgrades = buttons_number;

    newGeneration(true);
    // window.setMouseCursorVisible(true);

    return true;
}

// first thing to be called every frame
bool SBestReplay::onHandleEvent(GF::Event &event)
{
    static GF::ToggleKey T(sf::Keyboard::T);
    static GF::ToggleKey Y(sf::Keyboard::Y);
    static GF::ToggleKey H(sf::Keyboard::H);

    if (T.isKeyReleasedOnce(event) || !game.player.getActive())
        newGeneration(true);

    if (H.isKeyReleasedOnce(event))
        stateMachine->change("home");

    if (Y.isKeyReleasedOnce(event))
        newGeneration(true, 5);

    return true;
}

// called every frame before draw
bool SBestReplay::onUpdate(const float fElapsedTime, const float fTotalTime)
{
    if (1. / fElapsedTime < getMaxFPS() * 0.90)
        return true;

    /*    usleep(200000);

        VecD inputs = makeAIinput(game);
        VecD outputs = best->evaluate(inputs);
        std::cout << "INPUTS";
        std::cout << inputs << std::endl;
        std::cout << "OUTPUTS";
        std::cout << outputs << std::endl;*/

    auto info = stepAI(game, *best, 3. * fElapsedTime);
    float mouseAngle = info.first;
    bool shotshot = info.second;

    cursor.setPosition(game.player.getX() + CURSOR_RADIUS * cos(mouseAngle),
                       game.player.getY() + CURSOR_RADIUS * sin(mouseAngle));

    if (shotshot)
        bullet_sound.play();

    return true;
}

// last thing to be called every frame
bool SBestReplay::onDraw()
{
    renderer->draw(ground);

    game.player.draw(*renderer, paused);
    game.player.drawShots(*renderer);

    int digitsS =
        game.player.getScore() ? 1 + (int)log10(game.player.getScore()) : 0;
    int digitsM =
        game.player.getMoney() ? 1 + (int)log10(game.player.getMoney()) : 0;
    renderer->draw(
        GF::Text(std::to_string(game.player.getScore()),
                 sf::Vector2f(SW * (110.f + 10.f * digitsS), SH * 20.f),
                 (int)(SH * 20), BLACK, GF::TOPLEFT));
    renderer->draw(GF::Text("Score:", sf::Vector2f(SW * 50.f, SH * 20.f),
                            (int)(SH * 20), BLACK, GF::TOPLEFT));
    renderer->draw(
        GF::Text(std::to_string(game.player.getMoney()),
                 sf::Vector2f(SW * (35.f + 5.f * digitsM), SH * 54.f),
                 (int)(SH * 20), YELLOW, GF::TOPLEFT));

    for (int i = 0; i < buttons_number; i++)
        button[i].draw(*renderer);

    for (int i = 0; i < max_bots; i++)
    {
        game.bots[i].draw(*renderer);
    }

    renderer->draw(cursor);

    renderer->draw(dollarSign);

    renderer->draw(network_im);

    return true;
}

// called before exiting the app
void SBestReplay::onDestroy()
{
    if (image)
        delete image;
}
