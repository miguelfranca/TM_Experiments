#include "Application.h"

#include "Geodesic.hpp"
#include "Particle.hpp"
#include "Spacetime.hpp"

#include "Clock.hpp"
#include "ODEsolver.hpp"

Application::Application(std::string t)
{
	title = t;
	setStaticScreen(true);
}

// called once before the loop starts
bool Application::onCreate()
{
	double M = 0.;
	Schwarzschild sch(M);

	VecD position({10., M_PI / 2., 0.});
	double alpha = 0.;
	double beta = M_PI;
	double modV = 1.;

	Particle par(sch, position, alpha, beta, modV, -1.);
	auto mat = par.view(500);

	sky.loadFromFile("res/images/stars_1.jpg");
	sf::Image view;
	view.create(mat.getNC(), mat.getNL());

	for (unsigned l = 0; l < mat.getNL(); ++l) {
		std::cout << l << std::endl;
		std::cout << std::flush;
		for (unsigned c = 0; c < mat.getNC(); ++c) {
			auto vec = mat[l][c];
			sf::Color color;
			double r = vec[1];

			if (r < sch.BH_radius())
				color = BLACK;
			else {
				double theta = vec[2];
				double phi = vec[3];

				theta -= M_PI / 2;

				while (phi > M_PI) phi -= 2. * M_PI;

				while (phi <= -M_PI) phi += 2. * M_PI;

				double phi2 = phi * phi;
				double PI2 = M_PI * M_PI;
				double theta2 = theta * theta;

				double x = (phi < 0 ? -1 : 1) * M_PI * std::sqrt(phi2 * (PI2 - 4. * theta2) /
				           (PI2 * PI2 - 4. * theta2 * phi2));
				double y = (theta < 0 ? -1 : 1) * M_PI * std::sqrt(theta2 * (PI2 - phi2) /
				           (PI2 * PI2 - 4. * theta2 * phi2));

				sf::Vector2u size = sky.getSize();
				y = (y + M_PI / 2.) / M_PI * size.y;
				x = (x + M_PI) / (2. * M_PI) * size.x;

				color = sky.getPixel(x, y);
			}

			view.setPixel(c, l, color);
		}
	}

	std::cout << "FINISHED" << std::endl;

	sf::Texture txt;
	
	txt.loadFromImage(view);
	sf::Sprite sprt;
	
	sprt.setTexture(txt, true);
	sprt.setPosition(0, 0);
	
	double sx = ((double)SCREENWIDTH) / mat.getNC();
	double sy = ((double)SCREENHEIGHT) / mat.getNL();
	
	sx = std::min(sx, sy);
	sprt.scale(sx, sx);

	window.draw(sprt);
	return true;
}

// first thing to be called every frame
bool Application::onHandleEvent(GF::Event& event)
{
	return true;
}

// called every frame before draw
bool Application::onUpdate(const float fElapsedTime, const float fTotalTime)
{
	return true;
}

// last thing to be called every frame
bool Application::onDraw()
{
	return true;
}

// called before exiting the app
void Application::onDestroy()
{

}

void Application::onSwitch(std::string other)
{

}
