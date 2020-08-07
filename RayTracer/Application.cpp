#include "Application.h"

#include "Geodesic.hpp"
#include "Particle.hpp"
#include "Spacetime.hpp"

#include "Clock.hpp"
#include "ODEsolver.hpp"

#include "SkyMap.hpp"

Application::Application(std::string t)
{
	title = t;
	setStaticScreen(true);
	setupWindow(1000, 1000, 1000, 0);
}

// called once before the loop starts
bool Application::onCreate()
{
	return true;
}

// first thing to be called every frame
bool Application::onHandleEvent(GF::Event& event)
{
	// static double pan = M_PI / 180.;
	// static double pan = 0.;
	static double pan = M_PI / 180. * 6.5;
	static double dpan = M_PI / 180. / 2.;

	if(pan >= 2. * M_PI)
		return false;

	// Flat st;

	double M = 0.25;
	Schwarzschild st(M);

	// double M = 0.25;
	// double a = 0.;
	// Kerr st(M, a);

	VecD position({10., M_PI / 2., pan});
	double alpha = 0. / 180. * M_PI;
	double beta = 180. / 180. * M_PI;
	double modV = 1.;

	Particle par(st, position, alpha, beta, modV, -1.);
	par.setAngleViews(45. / 180. * M_PI, 45. / 180. * M_PI);
	auto mat = par.view(1000);
	// mat.print();

	ImageSkyMap sky(st, "res/images/stars_3.jpg", false);

	sf::Image view = sky.getSkyView(mat);

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

	// view.saveToFile("screenshot" + std::to_string(time(NULL)) + ".png");
	char buffer[30];
	sprintf(buffer,"walk_around_%05d.png",(int)std::round(pan*180./M_PI*100));
	view.saveToFile(buffer);

	pan += dpan;

	return true;
}

// called every frame before draw
bool Application::onUpdate(const float fElapsedTime, const float fTotalTime) { return true; }

// last thing to be called every frame
bool Application::onDraw() { return true; }

// called before exiting the app
void Application::onDestroy() {}

void Application::onSwitch(std::string other) {}
