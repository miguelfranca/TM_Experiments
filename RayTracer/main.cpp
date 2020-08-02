
#include "Geodesic.hpp"
#include "Particle.hpp"
#include "Spacetime.hpp"

#include "Clock.hpp"
#include "ODEsolver.hpp"

int main()
{
    double M = 1.;
    Schwarzschild sch(M);
    Geodesic geo(sch, -1); // -1 for particles, 0 for light

    VecD position({10., M_PI / 2., 0.});
    double alpha = 0.;
    double beta = M_PI;
    double modV = 1.;
    geo.shoot(position, alpha, beta, modV).print();

    return 0;
}