#pragma once

#include "Spacetime.hpp"

#include "ODEsolver.hpp"

VecD geodesic_RHS(const VecD &vars, const VecD &params, void *ptr);
bool geodesic_stopAtSingularity(const VecD &vars, const VecD &params, void *);

class Geodesic
{
  public:
    Geodesic(const Spacetime &a_st, double vel_squared);

    // static VecD make_vel3(double alpha, double beta, double modV);

    // angles must be sent in radians
    VecD shoot(const VecD &pos3, const VecD &vel3, bool evolveBackwards = false,
               bool showHistory = false) const;

    const Spacetime &st;
    const double V2; // -1 for massive particles, 0 for null rays

  private:
    ODEsolver ode;
};
