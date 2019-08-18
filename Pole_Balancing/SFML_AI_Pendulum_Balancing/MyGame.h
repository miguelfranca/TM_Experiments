#pragma once

#include <iostream>
#include"Game_Framework/main.h"
#include "Game_Framework/GUI.h"
#include "Game_Framework/sfml.h"
#include "Game_Framework/Tools.h"
#include "Pendulum.h"

class MyGame : public GF::Game {
public:

	MyGame(std::string t = "My Game!!!") { title = t; }

	// called once before the game starts
	bool onCreate() override
	{
		addWidget(new Pendulum(&window));
		return true;
	}

	// first thing to be called every frame
	bool onHandleEvent(GF::Event& event) override
	{
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

	void onDestroy(){
		clearWidgets();
	}

private:

};
