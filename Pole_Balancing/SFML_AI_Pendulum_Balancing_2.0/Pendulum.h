#pragma once
#include "Random.hpp"
#include "ODEsolver.hpp"
#include "NEAT_GA.hpp"
#include "Game_Framework/holders.h"
#include "Game_Framework/sfml.h"
#include "Game_Framework/main.h"
#include "Game_Framework/sfml/Orientation.h"

#define toRAD (M_PI/180.)
#define g 3
//9.8
#define M 1000.0
#define m 1.0
#define L 1.5
#define thetaMAX (36.*toRAD)
#define xMAX 2.5
#define thetaDMAX 1.5
#define xDMAX 1.0
#define A_MAGN 5.0

#define TMAX_SFML 30
#define SIM_SPEED 1.0

class Pendulum : public GF::Widget {
public:
	Pendulum(sf::RenderTarget* renderer = nullptr, const std::string indv_net = "", const sf::Color = sf::Color::White);
	Pendulum(sf::RenderTarget* renderer, const NEAT::Network& indv_net, const sf::Color);

	static VecD invertedPendulumRHS(const VecD& x, const VecD& params);
	static VecD step(VecD x0, double dt, const NEAT::Network& net, ODEsolver& eq, double extra_A = 0.);
	static VecD restart();

	bool handleEvent(GF::Event& event) override;

	virtual bool update(const float fElapsedTime, const float fTotalTime) override;
	virtual bool draw() override;

	void setAllowUserInput(bool);

public:
	bool isdead;

private:
	static double toPixels(double l);
	static double toMeters(double p);

	void move(float pos, float angle);

private:
	void init(sf::RenderTarget* renderer);

protected:
	sf::Texture base_tex;
	GF::Rectangle base; // base rectangle
	GF::Line line; // line that connects base to circle
	GF::Circle circle; // circle wieght

	ODEsolver eq;
	VecD x0;

	float m_lenght;
	bool allowUserInput;

	/////// NEAT ///////
	NEAT::Network net;
};
