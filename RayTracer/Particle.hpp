
#pragma once

#include "Matrix.hpp"

#include "Spacetime.hpp"

class Particle
{
  public:
    Particle(const Spacetime &a_st, const VecD &a_pos3, const VecD &a_vel3,
             double vel_squared)
        : st(a_st), pos3(a_pos3), vel3(a_vel3), V2(vel_squared),
          angle_H(M_PI / 4.), angle_V(M_PI / 4.)
    {
    }

    void setAngleViews(double horizontal, double vertical)
    {
        angle_H = horizontal;
        angle_V = vertical;
    }

    Matrix<VecD> view(unsigned n_rows, unsigned n_cols)
    {
        Geodesic geo(st, V2); // -1 for particles, 0 for light

        double xmax = tan(angle_H);
        double ymax = tan(angle_V);

        double dx = xmax * 2. / (n_cols - 1);
        double dy = ymax * 2. / (n_rows - 1);

        Matrix<VecD> mat(n_rows, n_cols, VecD());

        int i = 0, j = 0;
        for (double y = ymax; y >= -ymax; y -= dy)
        {
            for (double x = -xmax; x <= xmax; x += dx)
            {
                double alpha = atan(y / sqrt(x * x + 1.));
                double beta = atan(x);
                VecD end_point = geo.shoot(pos3, alpha, beta, 1., true);
                mat[i][j] = end_point;
                ++j;
            }
            ++i;
        }

        return mat;
    }

  private:
    const Spacetime &st;
    VecD pos3, vel3;
    double V2;

    double angle_H, angle_V;
};