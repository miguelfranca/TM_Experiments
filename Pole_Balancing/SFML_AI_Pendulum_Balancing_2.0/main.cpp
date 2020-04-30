#include "MyGame.h"

#include "GraphicsFramework/sfml/ToggleKey.h"
#include "Image.hpp"

#define TMAX (30. * 60.)
#define T_BOOST 10.
#define A_BOOST 1.
// #define dt 0.01
#define dt (1. / 60.)

// inputs and outputs normalized between [0,1]
double pole(const NEAT::Network &net, const GA::Evolver<NEAT::Network> *)
{
    VecD x0 = Pendulum::restart();

    ODEsolver eq(Pendulum::invertedPendulumRHS, 4);

    double t = 0.;
    // double a = 0.;
    double x02_mean = 0.;
    double x04_mean = 0.;
    unsigned steps = 0;

    while (t < TMAX && fabs(x0[1]) <= thetaMAX && fabs(x0[2]) <= xMAX)
    { // theta <= 36 degrees, x <= 2.5m
        x0 = Pendulum::step(x0, dt, net, eq);
        t += dt;
        // a += fabs(x0[1]);
        x02_mean += fabs(x0[2]);
        x04_mean += fabs(x0[4]);
        ++steps;
    }

    // a/=steps;
    // double penalty = fabs(x0[2])/xMAX;
    x02_mean /= steps;
    x04_mean /= steps;
    double penalty = fabs(x02_mean) / xMAX + fabs(x04_mean) / xDMAX;

    double F = t / TMAX * (T_BOOST - penalty) / T_BOOST * 100.;
    // double F = T_BOOST*t/TMAX - fabs(x0[1])/thetaMAX - fabs(x0[2])/xMAX -
    // fabs(x0[3])/thetaDMAX - fabs(x0[4])/xDMAX; double F = T_BOOST*t/TMAX -
    // A_BOOST*fabs(x0[1])/thetaMAX - fabs(x0[2])/xMAX; double F =
    // T_BOOST*t/TMAX - A_BOOST*a/thetaMAX - fabs(x0[2])/xMAX; double F =
    // T_BOOST*t/TMAX + A_BOOST/(0.25+a/thetaMAX) - fabs(x0[2])/xMAX;

    return F;
}

bool printPop(const Population<NEAT::Network> *pop)
{
    static MyGame gg;
    gg.updatePopulation(pop, 50);
    gg.showFPS(true);
    gg.setMaxFPS(60);

    static GF::ToggleKey P(sf::Keyboard::P);
    if (!P.isKeyPressed())
    {
        gg.run();
    }

    static GF::ToggleKey E(sf::Keyboard::E);
    if (E.isKeyPressed())
    {
        return false;
    }

    return true;
}

int main()
{
    NEAT::Evolver ga("params.txt");
    // ga.addCriteria(GA::StopReason::BestMaximum, {T_BOOST * 1});
    // ga.addCriteria(GA::StopReason::BestMaximum, {T_BOOST * 0.99});
    ga.addCriteria(GA::StopReason::BestMaximum, {99.}); // in percentage
    // ga.addCriteria(GA::StopReason::BestMaximum, {(T_BOOST+4.*A_BOOST) *
    // 0.99}); ga.addCriteria(GA::StopReason::BestMaximum, {TMAX});
    ga.setEvaluate(pole, GA::MAXIMIZE);
    ga.setPrintPopulation(printPop);

    ga.evolve();

    const NEAT::Network *best = &ga.getBest();
    best->print();
    print("Best Fitness = ", ga.getBestFitness());

    // pole2(*best,nullptr);
    // best->write("pole_best.txt");

    /*
        NEAT::Image image(*best, "params_image.txt");
        image.evolve();
        image.draw(700 * SW, 600 * SH);
        // image.save();
        image.wait();
    */

    return 0;
}
