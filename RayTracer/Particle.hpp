
#pragma once

#include "Matrix.hpp"

#include "Spacetime.hpp"

class Particle
{
  public:
    Particle(const Spacetime &a_st, const VecD &a_pos3, const VecD &vel3,
             double view_alpha, double view_beta, double vel_squared);

    void setAngleViews(double horizontal, double vertical);

    Matrix<VecD> view(unsigned points_horizontal);

  private:
    const Spacetime &st;
    VecD pos3, vel3;
    double alpha, beta, modV;
    double V2;

    double angle_H, angle_V;

    VecD make_velocity3(double alpha_light, double beta_light,
                        double modV_light);

    void calculate_tetrad();
    MatrixD tetrad;
};