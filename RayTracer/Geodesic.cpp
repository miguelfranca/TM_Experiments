
#include "Geodesic.hpp"

#include "Matrix.hpp"

Geodesic::Geodesic(const Spacetime &a_st, double vel_squared)
    : st(a_st), V2(vel_squared), ode(geodesic_RHS, 6)
{
    ode.setParams({V2});
    ode.setPointer(this);
    ode.setMethod(ODEsolver::RK45);
    ode.setStopCriteria(geodesic_stopAtSingularity);
}

// angles must be sent in radians
VecD Geodesic::shoot(const VecD &pos3, const VecD &vel3, bool evolveBackwards,
                     bool showHistory)
{
    VecD init({0.});
    init.insert(pos3);
    init.insert(vel3);

    // double dtau = 0.1 * (evolveBackwards ? -1. : 1.);
    double dtau = 0.01 * (evolveBackwards ? -1. : 1.);
    double tau = 1e8 * (evolveBackwards ? -1. : 1.);

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

VecD geodesic_RHS(const VecD &vars, const VecD &params, void *ptr)
{
    int i = 1;
    // 1st coordinate shouldn't matter, so sending a nan just to make sure
    // it really doesn't matter
    VecD x({NAN, vars[i++], vars[i++], vars[i++]});
    VecD v3({vars[i++], vars[i++], vars[i++]});

    Geodesic *geodesic = (Geodesic *)ptr;
    const Spacetime &st = geodesic->st;
    auto chris = st.christoffels(x);

    VecD v = st.velocity(x, v3, geodesic->V2);

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
    static double rMAX = 10000. * (BH_r == 0. ? 1. : BH_r);

    double t = vars[0];
    double r = vars[1];
    double vr = vars[4];
    return (r < BH_r * 0.95 &&
            vr * t <= 0 /*>0 if backwards, <0 if forward*/) ||
           r > rMAX || std::isnan(vr);
}
