
#pragma once

#include "Matrix.hpp"

#include "kokkos_macros.hpp"

class Spacetime
{
  public:
    KOKKOS_FUNCTION
    virtual MatrixD metric(const VecD &pos) const = 0;
    KOKKOS_FUNCTION
    virtual MatrixD imetric(const VecD &pos) const = 0;
    KOKKOS_FUNCTION
    virtual Vec<MatrixD> christoffels(const VecD &pos) const = 0;
    KOKKOS_FUNCTION
    virtual double
    BH_radius() const = 0; // informs of the radius of an existing BH

    // find the 0 component of the velocity to satisfy v_\mu v^\mu = V
    // V = -1 for massive particles
    // V = 0 for null rays
    KOKKOS_FUNCTION
    VecD velocity(VecD pos4, VecD vel3, double V) const;
};

class Flat : public Spacetime
{
  public:
    KOKKOS_FUNCTION
    Flat() {}

    KOKKOS_FUNCTION
    MatrixD metric(const VecD &pos) const override;
    KOKKOS_FUNCTION
    MatrixD imetric(const VecD &pos) const override;

    KOKKOS_FUNCTION
    Vec<MatrixD> christoffels(const VecD &pos) const override;

    KOKKOS_FUNCTION
    double BH_radius() const override
    {
        return 0.;
    } // informs of the radius of an existing BH
};

class Schwarzschild : public Spacetime
{
  public:
    KOKKOS_FUNCTION
    Schwarzschild(double a_M = 1.);

    KOKKOS_FUNCTION
    MatrixD metric(const VecD &pos) const override;
    KOKKOS_FUNCTION
    MatrixD imetric(const VecD &pos) const override;

    KOKKOS_FUNCTION
    Vec<MatrixD> christoffels(const VecD &pos) const override;

    KOKKOS_FUNCTION
    double
    BH_radius() const override; // informs of the radius of an existing BH

  private:
    const double M;

    KOKKOS_INLINE_FUNCTION
    inline double f(double r) const { return 1. - 2. * M / r; }
};

class Kerr : public Spacetime
{
  public:
    KOKKOS_FUNCTION
    Kerr(double a_M = 1., double a_a = 0.);

    KOKKOS_FUNCTION
    MatrixD metric(const VecD &pos) const override;
    KOKKOS_FUNCTION
    MatrixD imetric(const VecD &pos) const override;

    KOKKOS_FUNCTION
    Vec<MatrixD> christoffels(const VecD &pos) const override;

    KOKKOS_FUNCTION
    double
    BH_radius() const override; // informs of the radius of an existing BH

  private:
    const double M, a, a2;

    KOKKOS_INLINE_FUNCTION
    inline double f(double r, double sig) const
    {
        return 1. - 2. * M * r / sig;
    }
    KOKKOS_INLINE_FUNCTION
    inline double sigma(double r2, double costheta2) const
    {
        return r2 + a2 * costheta2;
    }
};
