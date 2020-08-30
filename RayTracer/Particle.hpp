
#pragma once

#include "Matrix.hpp"

#include "Spacetime.hpp"

#include "kokkos_macros.hpp"

class Particle
{
  public:
    Particle(const Spacetime &a_st, const VecD &a_pos3, const VecD &vel3,
             double view_alpha, double view_beta, double vel_squared);

    void setAngleViews(double horizontal, double vertical);

    Kokkos::View<double*** >::HostMirror view(unsigned points_horizontal);
    // Matrix<VecD> view(unsigned points_horizontal);

  private:
    const Spacetime &m_st;
    VecD m_pos3, m_vel3;
    VecD m_pos4, m_vel4;
    double m_alpha, m_beta;
    double m_V2;

    double m_angle_H, m_angle_V;

    KOKKOS_FUNCTION VecD make_velocity4(double alpha_light, double beta_light,
                        double modV_light);

    KOKKOS_FUNCTION double calculate_redshift(const VecD &vel4_ini, const VecD &end);

    void calculate_tetrad();
    MatrixD m_tetrad;
};