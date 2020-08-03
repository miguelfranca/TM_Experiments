
#include "Particle.hpp"
#include "Geodesic.hpp"

#include "ProgressBar.hpp"

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
    // shoot light rays
    Geodesic geo(st, 0); // -1 for particles, 0 for light

    unsigned points_vertical =
        std::round(points_horizontal * angle_V / angle_H);
    points_vertical = std::max((unsigned)2, points_vertical);

    Matrix<VecD> mat(points_vertical, points_horizontal, VecD());

    ProgressBar pb(points_vertical);

#pragma omp parallel for default(none)                                         \
    shared(pb, points_vertical, points_horizontal, geo, mat)
    for (unsigned l = 0; l < points_vertical; ++l)
    {
        // std::cout << ((double)l) / points_vertical * 100. << "%" <<
        // std::endl;
        for (unsigned c = 0; c < points_horizontal; ++c)
        {
            double alpha_light =
                angle_V - l * 2. * angle_V / (points_vertical - 1);
            double beta_light =
                -angle_H + c * 2. * angle_H / (points_horizontal - 1);

            VecD vel3 = make_velocity3(alpha_light, beta_light, 1.);

            // std::cout << "Vlast = (" << alpha_light * 180. / M_PI << ","
            // << beta_light * 180. / M_PI << ")" << std::endl;
            // std::cout << "Vlast = (" << alpha_light * 180. / M_PI << ","
            // << beta_light * 180. / M_PI << ")" << std::endl;

            // VecD vel3 = Geodesic::make_vel3(alpha_light, beta_light, 1.);
            // vel3.print();
            // VecD end_point = geo.shoot(pos3, vel3);
            VecD end_point = geo.shoot(pos3, vel3, true);
            // VecD end_point = geo.shoot(pos3, vel3, true, true);
            // VecD end_point =
            // geo.shoot(pos3, alpha_light, beta_light, 1., false, true);
            // geo.shoot(pos3, alpha_light, beta_light, 1., true);
            mat[l][c] = end_point;
        }
        ++pb;
    }

    return mat;
}

VecD Particle::make_velocity3(double alpha_light, double beta_light,
                              double modV_light)
{
    // alpha_light and beta_light: light angles from user reference point
    // alpha and beta: direction user is looking at
    modV_light = abs(modV_light);

    /*
        double vr_rot =
            modV * (cos(alpha) * cos(alpha_light) * cos(beta + beta_light) -
                    sin(alpha) * sin(alpha_light));

        double vtheta_rot =
            modV * (cos(alpha) * sin(alpha_light) * (cos(alpha) + cos(beta)) +
                    cos(alpha_light) * sin(alpha) *
                        (cos(beta_light) + cos(alpha) * cos(beta +
       beta_light))); if (vtheta_rot != 0.) vtheta_rot /= (1. + cos(alpha) *
       cos(beta));

        double tmp =
            modV * (sin(alpha) * (-cos(alpha) * sin(alpha_light) * sin(beta) +
                                  cos(alpha_light) * sin(alpha) *
       sin(beta_light))); double vphi_rot = modV * cos(alpha) * cos(alpha_light)
       * sin(beta + beta_light); if (tmp != 0.) vphi_rot += tmp / (1. +
       cos(alpha) * cos(beta));

    */
    /*
        VecD orig = {modV * cos(beta_light) * cos(alpha_light),
                     modV * sin(alpha_light),
                     modV * sin(beta_light) * cos(alpha_light)};
        VecD user = {cos(beta) * cos(alpha), sin(alpha), sin(beta) *
       cos(alpha)};

        double dangerous_den = 1. + user[0];

        double vr_rot = user[0] * orig[0] - user[1] * orig[1] - user[2] *
       orig[2]; double vtheta_rot = (user[0] + user[2] * user[2] /
       dangerous_den) * orig[1] + user[1] * orig[0] - user[1] * user[2] *
       orig[2] / dangerous_den; double vphi_rot = user[2] * orig[0] - user[1] *
       user[2] * orig[1] / dangerous_den + (user[0] + user[1] * user[1] /
       dangerous_den) * orig[2];

    */

    double vr_rot =
        cos(alpha) * cos(alpha_light) * cos(beta) * cos(beta_light) -
        cos(beta) * sin(alpha) * sin(alpha_light) -
        cos(alpha_light) * sin(beta) * sin(beta_light);
    double vtheta_rot = cos(alpha_light) * cos(beta_light) * sin(alpha) +
                        cos(alpha) * sin(alpha_light);
    double vphi_rot =
        cos(alpha) * cos(alpha_light) * cos(beta_light) * sin(beta) -
        sin(alpha) * sin(alpha_light) * sin(beta) +
        cos(alpha_light) * cos(beta) * sin(beta_light);

    VecD vel3_rot = {modV_light * vr_rot, modV_light * vtheta_rot,
                     modV_light * vphi_rot};

    // std::cout << std::endl;
    // std::cout << vel3_rot << std::endl;
    // std::cout << vel3_rot[0] * vel3_rot[0] + vel3_rot[1] * vel3_rot[1] +
    //                  vel3_rot[2] * vel3_rot[2]
    //           << std::endl;

    // 1st coordinate shouldn't matter, so sending a nan just to make
    // sure it really doesn't matter
    VecD pos4({NAN});
    pos4.insert(pos3);

    // convert to global coordinates
    // this is something temporary for E-F coordinates, not sure it stands in
    // general
    auto ig = st.imetric(pos4);
    vel3_rot[0] *= sqrt(ig[1][1]);
    vel3_rot[1] *= sqrt(ig[2][2]);
    vel3_rot[2] *= sqrt(ig[3][3]);

    // in the end, flip ray of light so that we can evolve it backwards
    vel3_rot[0] *= -1;
    vel3_rot[1] *= -1;
    vel3_rot[2] *= -1;

    return vel3_rot;
}
