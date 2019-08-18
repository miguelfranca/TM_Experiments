#include "Image.hpp"
#include "MyGame.h"
#include "Game_Framework/sfml/ToggleKey.h"

#define TMAX (30.)
#define T_BOOST 0
#define A_BOOST 1
// #define dt 0.01
#define dt (1./60.)

//inputs and outputs normalized between [0,1]
double pole(const NEAT::Network& net, const GA::Evolver<NEAT::Network>*){
	VecD x0 = Pendulum::restart();

	ODEsolver eq(Pendulum::invertedPendulumRHS, 4);

	double t = 0.;
	double a = 0.;
	unsigned c = 0;

	while(t<TMAX /*&& fabs(x0[1])<=thetaMAX */&& fabs(x0[2])<=xMAX){ //theta <= 36 degrees, x <= 2.5m
		x0 = Pendulum::step(x0, dt, net, eq);
		t += dt;
		a += fabs(x0[1]);
		++c;
	}

	a/=c;

	// double F = T_BOOST*t/TMAX - fabs(x0[1])/thetaMAX - fabs(x0[2])/xMAX - fabs(x0[3])/thetaDMAX - fabs(x0[4])/xDMAX;
	// double F = T_BOOST*t/TMAX - A_BOOST*fabs(x0[1])/thetaMAX - fabs(x0[2])/xMAX;
	// double F = T_BOOST*t/TMAX - A_BOOST*a/thetaMAX - fabs(x0[2])/xMAX;
	double F = T_BOOST*t/TMAX + A_BOOST/(0.25+a/thetaMAX) - fabs(x0[2])/xMAX;

	return F;
}

bool printPop(const Population<NEAT::Network>* pop){
	static MyGame gg;
	gg.updatePopulation(pop, 50);
	gg.showFPS(true);
	gg.setMaxFPS(60);

	static GF::ToggleKey P(sf::Keyboard::P);
	if(!P.isKeyPressed()){ 
		gg.run();
	}

	static GF::ToggleKey E (sf::Keyboard::E);
	if(E.isKeyPressed()){ 
		return false;
	}

	return true;
}

int main() {
	NEAT::Evolver ga("params.txt");
	// ga.addCriteria(GA::StopReason::BestMaximum, {T_BOOST * 1});
	// ga.addCriteria(GA::StopReason::BestMaximum, {T_BOOST * 0.99});
	ga.addCriteria(GA::StopReason::BestMaximum, {(T_BOOST+4.*A_BOOST) * 0.99});
	ga.setEvaluate(pole, GA::MAXIMIZE);
	ga.setPrintPopulation(printPop);

	ga.evolve();

	const NEAT::Network *best = &ga.getBest();
	best->print();
	print("Best Fitness = ",ga.getBestFitness());

	// pole2(*best,nullptr);
	best->write("pole_best.txt");

	NEAT::Image image(*best,"params_image.txt");
	image.evolve();
	image.draw();
	// image.save();
	image.wait();
	return 0;
}
