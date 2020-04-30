#pragma once
#include <iostream>

#include "GraphicsFramework/main.h"
#include "GraphicsFramework/GUI.h"
#include "GraphicsFramework/sfml.h"
// #include "GraphicsFramework/Tools.h"
#include "GraphicsFramework/Util.h"

#include "NeuralNetwork.hpp"

#define DEFAULT_RADIUS 40
#define MAX_RADIUS 100
#define MIN_RADIUS 2

class App : public GF::App
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
