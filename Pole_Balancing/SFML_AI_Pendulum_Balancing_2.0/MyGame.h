#pragma once

#include <iostream>
#include "Game_Framework/Tools.h"
#include "Game_Framework/main.h"
#include "Game_Framework/GUI.h"
#include "Game_Framework/sfml.h"
#include "Game_Framework/Global.h"
#include "Pendulum.h"
#include "UserDoublePendulum.h"
#include "UserPendulum.h"

class MyGame : public GF::Game {
public:

	MyGame() : image(nullptr), AI_Active(sf::Vector2f(0, 50 * SH)), time(sf::Vector2f(0, 200 * SH)), generation(sf::Vector2f(0, 100 * SH)),
	population(sf::Vector2f(0, 150 * SH)), fitness(sf::Vector2f(0, 250 * SH))
	{ this->pop = nullptr;}

	void updatePopulation(const Population<NEAT::Network>* pop, unsigned n = 1) { 
		this->pop = pop;
		N = n; 
		generation.setString("Generation: " + std::to_string(++generation_c));
	}

	// called once before the game starts
	bool onCreate() override
	{
		AI_Active.setString(game_state == 0 ? "Evolution" : (game_state == 1 ? "Best AI" : (game_state == 2 ? "Player - Double" : "Player - Single")));

		player_double = new UserDoublePendulum(&window);
		player_single = new UserPendulum(&window);
		BestAI = new Pendulum(&window, "../../../GAcpp/NEAT/v2/PoleBalancing_360/pole_best.txt");

		for(auto& pendulum : pendulums)
			delete pendulum;
		pendulums.clear();

		unsigned tot = 0;
		for(unsigned i=0; i<pop->species.size(); ++i)
			tot += pop->species[i]->pop.size();

		unsigned n = std::min(N, tot);

		unsigned iter = 0;
		while(n > pop->species.size()){
			for(unsigned i=0; i<pop->species.size(); ++i){
				if(pop->species[i]->bestRank == iter) continue;
				if(pop->species[i]->pop.size() <= iter) continue;
				pendulums.push_back(new Pendulum(&window, pop->species[i]->pop[iter]->I, sf::Color(255, 255, 255, 150)));
				--n;
			}
			++iter;
		}

		for(unsigned i=0; i<pop->species.size() && n>1; ++i){
			if(pop->bestSpecies != i) pendulums.push_back(new Pendulum(&window, pop->species[i]->getBest(), sf::Color(0, 0, 255, 150)));
			--n;
		}

		pendulums.push_back(new Pendulum(&window, pop->getBest(), sf::Color(0, 255, 0, 100)));

		std::cout << pop->species[ pop->bestSpecies ]->pop[ pop->species[ pop->bestSpecies ]->bestRank ]->fitness_orig << std::endl;

		const NEAT::Network *best = &pop->getBest();
		image = new NEAT::Image(*best,"params_image.txt");
		image->evolve();
		// image->draw(300. * SW, 200. * SH);
		network_im = image->makeImage(400 * SW, 400 * SH);
		network_im.setPosition(SCREENWIDTH - 420 * SW, 420 * SH);

		return true;
	}

	// first thing to be called every frame
	bool onHandleEvent(GF::Event& event) override
	{
		if(game_state == 0) 
			image->handleEvent();

		static GF::ToggleKey E (sf::Keyboard::E);
		if(E.isKeyPressed()){ 
			window.close();
			return false;
		}

		static GF::ToggleKey A (sf::Keyboard::A);
		if(A.isKeyPressedOnce(event)){ 
			game_state = 0;
			clearWidgets();
			AI_Active.setString("Evolution");
		}

		static GF::ToggleKey B (sf::Keyboard::B);
		if(B.isKeyPressedOnce(event)){ 
			game_state = 1;
			clearWidgets();
			AI_Active.setString("Best AI");
		}

		static GF::ToggleKey D (sf::Keyboard::D);
		if(D.isKeyPressedOnce(event)){ 
			game_state = 2;
			delete player_double;
			player_double = new UserDoublePendulum(&window);
			clearWidgets();
			AI_Active.setString("Player _ Double");
		}

		static GF::ToggleKey S (sf::Keyboard::S);
		if(S.isKeyPressedOnce(event)){ 
			game_state = 3;
			delete player_single;
			player_single = new UserPendulum(&window);
			clearWidgets();
			AI_Active.setString("Player _ Single");
		}

		if(game_state == 0){
			for(auto& pendulum : pendulums)
				if(!pendulum->isdead) pendulum->handleEvent(event);
		}

		if(game_state == 3)
			player_single->handleEvent(event);
		if(game_state == 2)
			player_double->handleEvent(event);
		if(game_state == 1)
			BestAI->handleEvent(event);

		return true;
	};

	// called every frame before draw
	bool onUpdate(const float fElapsedTime, const float fTotalTime) override
	{
		if(game_state == 0){

			bool out = false;
			for(auto& pendulum : pendulums){
				out |= pendulum->update(fElapsedTime, fTotalTime);
			}

			time.setString("Time: " + std::to_string(fTotalTime * SIM_SPEED));
			if(!out) image->close();
			return out;
		}

		if(game_state == 3)
			player_single->update(fElapsedTime, fTotalTime);

		if(game_state == 2)
			player_double->update(fElapsedTime, fTotalTime);

		if(game_state == 1)
			return BestAI->update(fElapsedTime, fTotalTime);

		return true;
	};

	// last thing to be called every frame
	bool onDraw() override
	{
		if(game_state == 0){
			unsigned population_c = 0;
			for(auto& pendulum : pendulums){
				if(!pendulum->isdead){
					++population_c;
					pendulum->draw();
				}
			}
			population.setString("Population size: " + std::to_string(population_c));
			fitness.setString("Best fitness : " + std::to_string(pop->getBestFitness()));

			window.draw(time);
			window.draw(generation);
			window.draw(population);
			window.draw(fitness);

			window.draw(network_im);
		}

		if(game_state == 3)
			player_single->draw();

		if(game_state == 2)
			player_double->draw();

		if(game_state == 1)
			BestAI->draw();

		window.draw(GF::Line(sf::Vector2f(0, SCREENHEIGHT - 100 * SH), sf::Vector2f(SCREENWIDTH, SCREENHEIGHT - 100 * SH)));

		window.draw(AI_Active);


		GF::Text text("E - Exit 	A - Evolution 	B - Best AI 	D - Player _ Double 	S - Player _ Single 	SPACE - Restart player 	ESC - Skip Generation (Evolution) 	(ESC + P) - Skip Generation fast",
			TOPLEFT_F + sf::Vector2f(650 * SW, 30 * SH),
			15 * SW);
		window.draw(text);
		window.draw(network_im);

		return true;
	};

	void onDestroy()
	{
		for(auto& pendulum : pendulums)
			delete pendulum;
		pendulums.clear();

		delete player_double;
		delete player_single;
		delete BestAI;

		if(image){
			delete image;
			image = nullptr;
		}

		clearWidgets();
	}

private:
	std::vector<Pendulum*> pendulums;
	const Population<NEAT::Network> *pop;
	NEAT::Image* image;
	sf::Sprite network_im;
	unsigned N; 

	// 0 = Trainging AI
	// 1 = Best AI running
	// 2 = player_double is playing
	int game_state = 0;

	GF::Text AI_Active;
	GF::Text time;
	GF::Text generation;
	GF::Text population;
	GF::Text fitness;
	unsigned generation_c = 0;


	UserDoublePendulum *player_double;
	UserPendulum *player_single;
	Pendulum *BestAI;
};
