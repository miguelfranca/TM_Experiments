
#include "Particle.hpp"
#include "Geodesic.hpp"

Particle::Particle(const Spacetime &a_st, const VecD &a_pos3, double a_alpha,
                   double a_beta, double a_modV, double vel_squared)
    : st(a_st), pos3(a_pos3), alpha(a_alpha), beta(a_beta), modV(a_modV),
      V2(vel_squared), angle_H(M_PI / 4.), angle_V(M_PI / 4.)
{
}

void Particle::setAngleViews(double horizontal, double vertical)
{
    angle_H = horizontal;
    angle_V = vertical;
}

Matrix<VecD> Particle::view(unsigned points_horizontal)
{
    Geodesic geo(st, V2); // -1 for particles, 0 for light

    double xmax = tan(angle_H);
    double ymax = tan(angle_V);

    unsigned points_vertical = points_horizontal * ymax / xmax;

    double dx = xmax * 2. / (points_horizontal - 1);
    double dy = ymax * 2. / (points_vertical - 1);

    Matrix<VecD> mat(points_vertical, points_horizontal, VecD());

    int i = 0, j = 0;
    for (double y = ymax; y >= -ymax; y -= dy)
    {
        for (double x = -xmax; x <= xmax; x += dx)
        {
            double alpha_light = atan(y / sqrt(x * x + 1.));
            double beta_light = atan(x);

            // invert light ray
            alpha_light = -alpha_light;
            beta_light = beta_light + M_PI;

            // add particle alpha and beta
            alpha_light += alpha;
            beta_light += beta;

            // normalize to correct domain
            if (alpha_light > M_PI / 2.)
            {
                alpha_light = M_PI - alpha_light;
                beta_light += M_PI;
            }
            if (alpha_light < -M_PI / 2.)
            {
                alpha_light = -M_PI - alpha_light;
                beta_light += M_PI;
            }
            while (beta_light > M_PI)
                beta_light -= 2. * M_PI;
            while (beta_light < -M_PI)
                beta_light += 2. * M_PI;

            // std::cout << "Vlast = (" << alpha_light * 180. / M_PI << ","
            // << beta_light * 180. / M_PI << ")" << std::endl;

            VecD vel3 = Geodesic::make_vel3(alpha_light, beta_light, 1.);
            VecD end_point = geo.shoot(pos3, vel3, true);
            mat[i][j] = end_point;
            ++j;
        }
        ++i;
        j = 0;
    }

    return mat;
}
