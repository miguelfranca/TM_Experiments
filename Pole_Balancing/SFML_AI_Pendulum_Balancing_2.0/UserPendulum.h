#pragma once
#include "Game_Framework/Tools/Random.hpp"
#include "Game_Framework/Tools/ODEsolver.hpp"
#include "Game_Framework/holders.h"
#include "Game_Framework/sfml.h"
#include "NEAT_GA.hpp"
#include "Image.hpp"
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

class UserPendulum : public GF::Widget {
public:
	static double toPixels(double l){ return l/(2.*xMAX)*SCREENWIDTH; }
	static double toMeters(double p){ return p*(2.*xMAX)/SCREENWIDTH; }
	static VecD invertedPendulumRHS(const VecD& x, const VecD& params);
	void restart();

	UserPendulum(sf::RenderTarget* renderer = nullptr);

	bool handleEvent(GF::Event& event) override;

	virtual bool update(const float fElapsedTime, const float fTotalTime) override;
	virtual bool draw() override;
private:
	void move(float pos, float angle);
protected:
	sf::Texture base_tex;
	GF::Rectangle base; // base rectangle
	GF::Line line; // line that connects base to circle
	GF::Circle circle; // circle wieght

	ODEsolver eq;
	VecD x0;

	float m_lenght;
};
