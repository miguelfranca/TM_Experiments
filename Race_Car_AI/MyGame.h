#pragma once

#include <iostream>
#include "Game_Framework/main.h"
#include "Game_Framework/GUI.h"
#include "Game_Framework/sfml.h"
#include "Game_Framework/Tools.h"
#include "NEAT_GA.hpp"


#include "Car.h"
#include "Track.h"

class MyGame : public GF::Game {
public:

	MyGame(std::string t = "My Game!!!") : net("params.txt") { title = t; }

	// called once before the game starts
	bool onCreate() override
	{
		Track* track = new Track(&window);
		addWidget(track, "track");

		addWidget(new Car(&window, CENTER_WINDOW, 40, 80, track, "res/textures/car0.png"), "car");

		setClearColor(sf::Color::White);

		return true;
	}

	// first thing to be called every frame
	bool onHandleEvent(GF::Event& event) override
	{

		static bool being_dragged = false;
		static Car* car = (Car*)getWidget("car");
		static Track* track = (Track*)getWidget("track");

		if(!window.hasFocus() || event.type == sf::Event::GainedFocus) return true;

		if(GF::Mouse::Left.clicked(event) && car->isClicked())
			being_dragged = true;

		if(being_dragged && GF::Mouse::Left.isPressed() && car->isClicked())
			car->setPosition(GF::Mouse::getPosition(window));

		if(!car->isClicked() || GF::Mouse::Left.released(event))
			being_dragged = false;

		if(GF::Mouse::Left.isPressed() && !being_dragged)
			track->addCircle(GF::Mouse::getPosition(window));

		return true;
	};

	// called every frame before draw
	bool onUpdate(const float fElapsedTime, const float fTotalTime) override
	{
		return true;
	};

	// last thing to be called every frame
	bool onDraw() override
	{
		return true;
	};

	void onDestroy()
	{

	}

private:

	NEAT::Network net;

};
