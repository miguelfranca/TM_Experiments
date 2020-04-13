#pragma once

#include <iostream>

#include "Game_Framework/main.h"
#include "Game_Framework/GUI.h"
#include "Game_Framework/sfml.h"
#include "Game_Framework/Tools.h"

#include "NEAT_GA.hpp"
#include "Image.hpp"

#include "Car.h"
#include "Track.h"


#define INPUT_FACTOR 4.

static bool alive_condition(const sf::Image& track_im, const Track* track, const Car* car){
	return track_im.getPixel(car->getPosition().x, SCREENHEIGHT - car->getPosition().y) == GRAY;
}

class CarAI : public GF::Game {
public:

	CarAI(std::string t = "My Game!!!");

	// called once before the game starts
	bool onCreate() override;

	bool handle_Track_Car_events(GF::Event &event);

	// first thing to be called every frame
	bool onHandleEvent(GF::Event& event) override;

	// called every frame before draw
	bool onUpdate(const float fElapsedTime, const float fTotalTime) override;

	// last thing to be called every frame
	bool onDraw() override;

	void onDestroy();

	void onSwitch(std::string) {}

private:
	void reset();

	void calculate_size();
private:

	NEAT::Image* image;
	sf::Sprite network_im;
	Car* AI;

	bool pause;
	NEAT::Network* AI_NN;
	
};
