#pragma once

#include "Matrix.hpp"
#include "ODEsolver.hpp"

#include "Spacetime.hpp"

VecD geodesic_RHS(const VecD &vars, const VecD &params, void *ptr);
bool geodesic_stopAtSingularity(const VecD &vars, const VecD &params, void *);

class Geodesic
{
  public:
    Geodesic(const Spacetime &a_st, double vel_squared)
        : st(a_st), V2(vel_squared)
    {
    }

    // angles must be sent in radians
    VecD shoot(const VecD &pos3, double alpha, double beta, double modV,
               bool evolveBackwards = false, bool showHistory = false)
    {
        // 1st coordinate shouldn't matter, so sending a nan just to make sure
        // it really doesn't matter
        VecD pos4({NAN});
        pos4.insert(pos3);

        modV = abs(modV);

        VecD vel3({modV * cos(beta) * cos(alpha), modV * sin(alpha),
                   modV * sin(beta) * cos(alpha)}); // from rest

        auto vel4 = st.velocity(pos4, vel3, V2);
        // vel4.print();
        // std::cout << std::endl;
        if (std::isnan(vel4[0]) or vel4.norm() == 0. /* they are all 0*/)
        {
            throw std::runtime_error(
                "3 velocity invalid for position and particle type chosen.");
            return pos4;
        }

        VecD init({0.});
        init.insert(pos3);
        init.insert(vel3);

        ODEsolver ode(geodesic_RHS, 6);
        ode.setParams({V2});
        ode.setPointer(this);
        // ode.setMethod(ODEsolver::RK45);
        ode.setStopCriteria(geodesic_stopAtSingularity);

        double dtau = 0.01 * (evolveBackwards ? -1. : 1.);
        double tau = 1e6 * (evolveBackwards ? -1. : 1.);

        // int N = 100000;
        // Clock c;
        // MatrixD m;
        // FOR(N) { m = ode.solve(init, tau, dtau, 1.e-5); }
        // m.print();
        // c("t = ", "s\n");

        VecD v;
        if (showHistory)
        {
            MatrixD m = ode.evolve(init, tau, dtau, 1.e-5);
            m.print();
            v = m.back();
        }
        else
            v = ode.solve(init, tau, dtau, 1.e-5);

        return v;
    }

    const Spacetime &st;
    const double V2; // -1 for massive particles, 0 for null rays
};

VecD geodesic_RHS(const VecD &vars, const VecD &params, void *ptr)
{
    int i = 1;
    // 1st coordinate shouldn't matter, so sending a nan just to make sure it
    // really doesn't matter
    VecD x({NAN, vars[i++], vars[i++], vars[i++]});
    VecD v3({vars[i++], vars[i++], vars[i++]});

    Geodesic *geodesic = (Geodesic *)ptr;
    const Spacetime &st = geodesic->st;
    auto chris = st.christoffels(x);

    double V2 = geodesic->V2;
    VecD v = st.velocity(x, v3, V2);

    VecD rhs(7);
    rhs[0] = 1;
    FOR(i, 3) { rhs[i + 1] = v3[i]; }
    FOR(i, 3) { rhs[i + 1 + 3] = -chris[i + 1].prod(v).dot(v); }

    // rhs.print();
    return rhs;
}

bool geodesic_stopAtSingularity(const VecD &vars, const VecD &params, void *ptr)
{
    Geodesic *geodesic = (Geodesic *)ptr;
    double BH_r = geodesic->st.BH_radius();
    static double rMAX = 100. * BH_r;

    double r = vars[1];
    double vr = vars[4];
    return r < BH_r * 0.05 || r > rMAX || std::isnan(vr);
}
