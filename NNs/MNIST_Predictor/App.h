#pragma once
#include <iostream>

#include "Game_Framework/main.h"
#include "Game_Framework/GUI.h"
#include "Game_Framework/sfml.h"
// #include "Game_Framework/Tools.h"
#include "Game_Framework/Util.h"

#include "NeuralNetwork.hpp"

#define DEFAULT_RADIUS 40
#define MAX_RADIUS 100
#define MIN_RADIUS 2

class App : public GF::Game
{
public:

	App(const std::string title = "MNIST Preditctor");

	// called once before the game starts
	bool onCreate() override;

	// first thing to be called every frame
	bool onHandleEvent(GF::Event& event) override;

	// called every frame before draw
	bool onUpdate(const float fElapsedTime, const float fTotalTime) override;

	// last thing to be called every frame
	bool onDraw() override;

	// called before app is terminated
	void onDestroy() override;

	// called when switching between game-states
	void onSwitch(std::string) override;
private:
	NN::NeuralNetwork net;
	unsigned radius;
};
