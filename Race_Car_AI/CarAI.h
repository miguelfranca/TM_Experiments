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

#define dt (1./60.) // seg
#define TIME_BOOST 100.
#define CHECKPOINT_BOOST 5.
#define LAPS 3.
#define TIME_BETWEEN_CHECKPOINTS 3. // seg

#define INPUT_FACTOR 4.
#define DRAW_SIZE 50

class CarEvolver : public NEAT::Evolver {
public:
	CarEvolver(const std::string params) : NEAT::Evolver(params) {}
	Track* track;
};

static bool alive_condition(const sf::Image& track_im, const Track* track, const Car* car){
	return track_im.getPixel(car->getPosition().x, SCREENHEIGHT - car->getPosition().y) == GRAY 
	&& car->t < (car->getCheckpoint() * TIME_BETWEEN_CHECKPOINTS + TIME_BETWEEN_CHECKPOINTS) && car->getCheckpoint() < track->getMaxCheckpoint()*LAPS;
}

static double fitnessFunc (const NEAT::Network& net, const GA::Evolver<NEAT::Network>* ga)
{
	Track* track = ((CarEvolver*)ga)->track;

	Car car (nullptr, track->start_pos(), 40, 80, track, "");
	car.setRotation(track->start_rotation());
	car.update(0., 0.);

	VecD inputs;

	static sf::Image &track_im = track->getTrack();
	// track_im.flipVertically();

	while(alive_condition(track_im, track, &car))
	{
		car.t += dt;
		inputs = car.getSensors();

		inputs *= INPUT_FACTOR/SCREENWIDTH;
		VecD outputs = net.evaluate(VecD(inputs));

		car.steer_gas(outputs[0] * 2. - 1., outputs[1] * 2. - 1.);

		car.update(dt, car.t);
		// std::cout << car.getPosition() << car.getRotation() << std::endl;
		// std::cout << t << "\t" << car.getCheckpoint() << std::endl;
		// std::cout << inputs << outputs << std::endl;
	}


	return TIME_BOOST*(double)car.getCheckpoint()/track->getMaxCheckpoint() - CHECKPOINT_BOOST*car.t/LAPS/track->getMaxCheckpoint();
	// return (double)car.getCheckpoint() / car.t;
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

private:
	void reset();

	void calculate_size();
private:

	CarEvolver ga;
	GA::StopReason stop;
	NEAT::Image* image;
	sf::Sprite network_im;
	double lastFitness;

	bool drawing;
	bool ready_for_evolution;

	Car* AI;

	unsigned N;

	std::vector<Car*> cars;
	bool pause;
};
