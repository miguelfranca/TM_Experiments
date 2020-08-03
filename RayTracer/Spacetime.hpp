
#pragma once

#include "Matrix.hpp"

class Spacetime
{
  public:
    virtual MatrixD metric(const VecD &pos) const = 0;
    virtual MatrixD imetric(const VecD &pos) const = 0;
    virtual Vec<MatrixD> christoffels(const VecD &pos) const = 0;
    virtual double
    BH_radius() const = 0; // informs of the radius of an existing BH

    // find the 0 component of the velocity to satisfy v_\mu v^\mu = V
    // V = -1 for massive particles
    // V = 0 for null rays
    VecD velocity(VecD pos4, VecD vel3, double V) const;
};

class Flat : public Spacetime
{
  public:
    Flat() {}

    MatrixD metric(const VecD &pos) const override;
    MatrixD imetric(const VecD &pos) const override;

    Vec<MatrixD> christoffels(const VecD &pos) const override;

    double BH_radius() const override
    {
        return 0.;
    } // informs of the radius of an existing BH
};

class Schwarzschild : public Spacetime
{
  public:
    Schwarzschild(double a_M = 1.);

    MatrixD metric(const VecD &pos) const override;
    MatrixD imetric(const VecD &pos) const override;

    Vec<MatrixD> christoffels(const VecD &pos) const override;

    double
    BH_radius() const override; // informs of the radius of an existing BH

  private:
    const double M;

    inline double f(double r) const { return 1. - 2. * M / r; }
};
