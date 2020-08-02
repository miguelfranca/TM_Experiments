
#include "ODEsolver.hpp"

VecD friction(const VecD &vars, const VecD &params)
{
    double v = vars[2];

    VecD rhs(3);
    rhs[0] = 1;
    rhs[1] = v;
    rhs[2] = -0.5 * v;

    return rhs;
}

int main()
{
    VecD init({0., 0., 5.});
    ODEsolver ode(friction, 2);
    ode.setMethod(ODEsolver::RK45);

    double dt = 0.1;
    double T = 10.;

    auto m = ode.solve(init, T, dt, 1.e-5);

    m.print();

    // auto m2 = ode.solve(m.back(), 0, -dt);

    // m2.print();

    return 0;
}