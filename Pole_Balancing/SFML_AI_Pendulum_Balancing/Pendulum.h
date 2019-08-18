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

class Pendulum : public GF::Widget {
public:
	static double toPixels(double l){ return l/(2.*xMAX)*SCREENWIDTH; }
	static double toMeters(double p){ return p*(2.*xMAX)/SCREENWIDTH; }
	static VecD invertedPendulumRHS(const VecD& x, const VecD& params) {
		double A = params[0];

		return VecD({ 1,
			x[3],
			x[4],
			((m * g * sin(x[1]) * cos(x[1]) - m * L * x[3] * x[3] * sin(x[1]) + A * (M+m) ) / (M + m * sin(x[1]) * sin(x[1])) * cos(x[1]) + g * sin(x[1])) / L,
			(m * g * sin(x[1]) * cos(x[1]) - m * L * x[3] * x[3] * sin(x[1]) + A * (M+m) ) / (M + m * sin(x[1]) * sin(x[1]))
		});
	}

	void restart(){
		x0[0] = 0.;
		x0[1] = 30*toRAD;
		x0[2] = 0.;
		x0[3] = 0.;
		x0[4] = 0.;
	// 	x0[1] = Rand(-thetaMAX, thetaMAX) * 0.5;
	// 	x0[2] = Rand(-xMAX, xMAX) * 0.95; //make sure it starts inside the track
	// 	x0[3] = Rand(-thetaDMAX, thetaDMAX);
	// 	x0[4] = Rand(-xDMAX, xDMAX) * 0.;
	}


	Pendulum(sf::RenderTarget* renderer = nullptr) : 
		base(sf::Vector2f(300 * SW, 100 * SH), CENTER_WINDOW + sf::Vector2f(0, 200 * SH), GF::CENTERED), // setup base car
		line(sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH)), sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L)))), // setup line
		circle(15, sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L)))), // setup circle

		eq(Pendulum::invertedPendulumRHS, 4),
		x0(5),

	m_lenght(toPixels(L)), // lenght of pendulum
	// net("/home/miguel/Desktop/Miguel França_Universidade/Programing/SFML/GAcpp/NEAT/v2/PoleBalancing_360/pole_best.txt")
	net("/home/miguel/Desktop/Miguel França_Universidade/Programing/SFML/Pole_Balancing/SFML_AI_Pendulum_Balancing_2.0/pole_best.txt")
	{
		base_tex.loadFromFile("res/textures/truck.jpg");
		base.setTexture(&base_tex);
		m_target = renderer;

		restart();
		move(x0[2],x0[1]);
	}

	bool handleEvent(GF::Event& event) override
	{
		static GF::ToggleKey SPACE(sf::Keyboard::Space);
		static GF::ToggleKey A(sf::Keyboard::A);
		if (SPACE.isKeyPressedOnce(event))
			restart();
		if (A.isKeyPressedOnce(event))
			AI_active = !AI_active;

		return true;
	}

	virtual bool update(const float fElapsedTime, const float fTotalTime) override
	{
		static GF::ToggleKey LEFT(sf::Keyboard::Left);
		static GF::ToggleKey RIGHT(sf::Keyboard::Right);

		float A = 0.;
		if (LEFT.isKeyPressed())
			A = -A_MAGN;
		if (RIGHT.isKeyPressed())
			A = A_MAGN;

		while(x0[1]>2.*M_PI)  x0[1] -= 4.*M_PI;
		while(x0[1]<-2.*M_PI) x0[1] += 4.*M_PI;

		double AA = 0.;
		if(AI_active){
			VecD inputs(4);
			inputs[0] = (x0[1]/thetaMAX 	+ 1.)/2.;
			inputs[1] = (x0[2]/xMAX 		+ 1.)/2.;
			inputs[2] = (x0[3]/thetaDMAX 	+ 1.)/2.;
			inputs[3] = (x0[4]/xDMAX 		+ 1.)/2.;
			AA = A_MAGN * (2*(int)(net.evaluate(inputs)[0]+0.5)-1);
		}
		eq.setParams({ AA + A });
		x0 = eq.stepRK4(x0, fElapsedTime);

		move(x0[2],x0[1]);

		return true;
	}

	virtual bool draw() override
	{
		m_target->draw(base);
		m_target->draw(line);
		m_target->draw(circle);
		m_target->draw(GF::Line(sf::Vector2f(0, SCREENHEIGHT - 100 * SH), sf::Vector2f(SCREENWIDTH, SCREENHEIGHT - 100 * SH)));
		GF::Text text(AI_active ? "AI active" : "AI not active", sf::Vector2f(0, 40), DEFAULT_CHAR_SIZE, AI_active ? sf::Color::Green : sf::Color::Red, GF::Alignment::TOPLEFT);
		text.draw(*m_target, sf::RenderStates::Default);
		if(!AI_active){
			float minX = std::max(base.getGlobalBounds().width / 2., toPixels(L) - base.getGlobalBounds().width / 4.);
			float maxX = SCREENWIDTH - std::max(base.getGlobalBounds().width / 2., toPixels(L) + base.getGlobalBounds().width / 4.);

			int sign = (base.getPosition().x <= minX) ? 1 : -1;

			if(base.getPosition().x <= minX || base.getPosition().x >= maxX){
				if(base.getPosition().x <= 0. || base.getPosition().x >= SCREENWIDTH)
					x0[2] += sign*2*xMAX;

				base.setPosition((int)SCREENWIDTH*sign + base.getPosition().x, base.getPosition().y);
				line.line[0].position.x = (int)SCREENWIDTH*sign + line.line[0].position.x;
				line.line[1].position.x = (int)SCREENWIDTH*sign + line.line[1].position.x;
				circle.setPosition((int)SCREENWIDTH*sign + circle.getPosition().x, circle.getPosition().y);
				m_target->draw(base);
				m_target->draw(line);
				m_target->draw(circle);
			}
		}

		return true;
	}

private:
	void move(float pos, float angle) {
		pos = toPixels(pos+xMAX)-base.getGlobalBounds().width/4.;

		base.setPosition(pos, base.getPosition().y);
		line.line[0].position.x = pos + base.getGlobalBounds().width / 4;
		line.line[1].position.x = line.line[0].position.x - m_lenght * sin(angle);
		line.line[1].position.y = line.line[0].position.y - m_lenght * cos(angle);

		circle.setPosition(line.line[1].position);
	}

protected:
	sf::Texture base_tex;
	GF::Rectangle base; // base rectangle
	GF::Line line; // line that connects base to circle
	GF::Circle circle; // circle wieght

	ODEsolver eq;
	VecD x0;

	float m_lenght;



	/////// NEAT ///////
	NEAT::Network net;
	bool AI_active = true;
};
