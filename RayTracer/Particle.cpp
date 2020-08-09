
#include "Particle.hpp"
#include "Geodesic.hpp"

#include "ProgressBar.hpp"

Particle::Particle(const Spacetime &a_st, const VecD &a_pos3,
                   const VecD &a_vel3, double a_alpha, double a_beta,
                   double vel_squared)
    : m_st(a_st), m_pos3(a_pos3), m_vel3(a_vel3), m_alpha(a_alpha),
      m_beta(a_beta), m_V2(vel_squared), m_angle_H(M_PI / 4.),
      m_angle_V(M_PI / 4.), m_tetrad(4, 4, 0.)
{
    m_pos4 = {NAN, m_pos3[0], m_pos3[1], m_pos3[2]};
    m_vel4 = m_st.velocity(m_pos4, m_vel3, m_V2);

    calculate_tetrad();
}

void Particle::setAngleViews(double horizontal, double vertical)
{
    m_angle_H = horizontal;
    m_angle_V = vertical;
}

void Particle::calculate_tetrad()
{
    auto g = m_st.metric(m_pos4);
    const VecD &et = m_vel4;
    VecD et_down = g.prod(et);

    VecD vr = {0., 1., 0., 0.};
    VecD vq = {0., 0., 1., 0.};
    VecD vf = {0., 0., 0., 1.};

    VecD proj_et_vq = -(et_down.dot(vq)) * et;
    VecD eq = vq - proj_et_vq;
    eq = eq / sqrt(g.prod(eq).dot(eq));
    VecD eq_down = g.prod(eq);

    VecD proj_et_vf = -(et_down.dot(vf)) * et;
    VecD proj_eq_vf = (eq_down.dot(vf)) * eq;
    VecD ef = vf - proj_et_vf - proj_eq_vf;
    ef = ef / sqrt(g.prod(ef).dot(ef));
    VecD ef_down = g.prod(ef);

    VecD proj_et_vr = -(et_down.dot(vr)) * et;
    VecD proj_eq_vr = (eq_down.dot(vr)) * eq;
    VecD proj_ef_vr = (ef_down.dot(vr)) * ef;
    VecD er = vr - proj_et_vr - proj_eq_vr - proj_ef_vr;
    er = er / sqrt(g.prod(er).dot(er));
    // VecD er_down = g.prod(er);

    m_tetrad[0] = et;
    m_tetrad[1] = er;
    m_tetrad[2] = eq;
    m_tetrad[3] = ef;

    m_tetrad = m_tetrad.transpose();

    /*
        std::cout << g.prod(et).dot(et) << std::endl;
        std::cout << g.prod(er).dot(er) << std::endl;
        std::cout << g.prod(eq).dot(eq) << std::endl;
        std::cout << g.prod(ef).dot(ef) << std::endl;
        std::cout << g.prod(et).dot(er) << std::endl;
        std::cout << g.prod(et).dot(eq) << std::endl;
        std::cout << g.prod(et).dot(ef) << std::endl;
        std::cout << g.prod(er).dot(eq) << std::endl;
        std::cout << g.prod(er).dot(ef) << std::endl;
        std::cout << g.prod(eq).dot(ef) << std::endl;
        m_tetrad.print();
        throw std::runtime_error("stop");
    */
}

Matrix<VecD> Particle::view(unsigned points_horizontal)
{
    // shoot light rays
    Geodesic geo(m_st, 0); // -1 for particles, 0 for light

    unsigned points_vertical =
        std::round(points_horizontal * m_angle_V / m_angle_H);
    points_vertical = std::max((unsigned)2, points_vertical);

    Matrix<VecD> mat(points_vertical, points_horizontal, VecD());

    ProgressBar pb(points_vertical);

    double dV = 2. * m_angle_V / (points_vertical - 1);
    double dH = 2. * m_angle_H / (points_horizontal - 1);

#pragma omp parallel for default(none)                                         \
    shared(pb, points_vertical, points_horizontal, geo, mat, dV, dH)
    for (unsigned l = 0; l < points_vertical; ++l)
    {
        for (unsigned c = 0; c < points_horizontal; ++c)
        {
            double alpha_light = m_angle_V - l * dV;
            double beta_light = -m_angle_H + c * dH;

            VecD vel4 = make_velocity4(alpha_light, beta_light, 1.);
            VecD vel3 = {vel4[1], vel4[2], vel4[3]};

            VecD end_point = geo.shoot(m_pos3, vel3, true);
            // VecD end_point = geo.shoot(pos3, vel3, true, true);
            double redshift = calculate_redshift(vel4, end_point);
            end_point.push_back(redshift);

            mat[l][c] = end_point;
        }
        ++pb;
    }

    return mat;
}

VecD Particle::make_velocity4(double alpha_light, double beta_light,
                              double modV_light)
{
    // alpha_light and beta_light: light angles from user reference point
    // alpha and beta: direction user is looking at
    modV_light = abs(modV_light);

    double cosa = cos(m_alpha);
    double cosal = cos(alpha_light);
    double sina = sin(m_alpha);
    double sinal = sin(alpha_light);
    double cosb = cos(m_beta);
    double cosbl = cos(beta_light);
    double sinb = sin(m_beta);
    double sinbl = sin(beta_light);

    double vr_rot = cosa * cosal * cosb * cosbl - cosb * sina * sinal -
                    cosal * sinb * sinbl;
    double vtheta_rot = cosal * cosbl * sina + cosa * sinal;
    double vphi_rot = cosa * cosal * cosbl * sinb - sina * sinal * sinb +
                      cosal * cosb * sinbl;

    VecD vel3_rot = {modV_light * vr_rot, modV_light * vtheta_rot,
                     modV_light * vphi_rot};
    // vel3_rot.print();

    // std::cout << std::endl;
    // std::cout << vel3_rot << std::endl;
    // std::cout << vel3_rot[0] * vel3_rot[0] + vel3_rot[1] * vel3_rot[1] +
    //                  vel3_rot[2] * vel3_rot[2]
    //           << std::endl;

    // the right one is negative. This is not negative time. It's in 'v'
    // coordinates. I'm slightly confused to why this works
    // double vel4_0 = -(vel3_rot[0] > 0 ? 1. : -1.) *
    double vel4_0 = sqrt(vel3_rot[0] * vel3_rot[0] + vel3_rot[1] * vel3_rot[1] +
                         vel3_rot[2] * vel3_rot[2]);
    // flip spatial part of ray of light so that we can evolve it backwards
    VecD vel4_user = {vel4_0, -vel3_rot[0], -vel3_rot[1], -vel3_rot[2]};

    // tetrad.print();

    VecD vel4 = m_tetrad.prod(vel4_user);

    // convert to global coordinates
    // this assumes the metric is of type (g00 g01 g10 g11) x (g22 g33), not
    // sure it stands in general
    // auto ig = st.imetric(pos4);
    // vel3_rot[0] *= sqrt(std::abs(ig[1][1]));
    // vel3_rot[1] *= sqrt(std::abs(ig[2][2]));
    // vel3_rot[2] *= sqrt(std::abs(ig[3][3]));

    // in the end, flip ray of light so that we can evolve it backwards
    // vel3_rot[0] *= -1;
    // vel3_rot[1] *= -1;
    // vel3_rot[2] *= -1;
    // vel3[0] *= -1;
    // vel3[1] *= -1;
    // vel3[2] *= -1;

    // 1st coordinate shouldn't matter, so sending a nan just to make
    // sure it really doesn't matter
    // VecD pos4({NAN});
    // pos4.insert(pos3);
    // std::cout << "VEL4" << std::endl;
    // vel4.print();
    // st.velocity(pos4, vel3, 0).print();
    // std::cout << std::endl;
    if (vel4.norm() == 0. /* they are all 0*/)
    {
        throw std::runtime_error(
            "3 velocity invalid for position and particle type chosen.");
        return vel4;
    }
    // geodesic invalid (probably inside BH in an orbit that is invalid, for
    // example too big angles, just let it be and assume it collides to
    // singularity)
    if (std::isnan(vel4[0]))
    {
        throw std::runtime_error("Check me.");
        return vel4;
    }

    return vel4;
}

double Particle::calculate_redshift(const VecD &vel4_init, const VecD &end)
{
    auto g_init = m_st.metric(m_pos4);

    VecD pos4_end = {NAN, end[1], end[2], end[3]};
    auto g_end = m_st.metric(pos4_end);

    VecD vel3_end = {end[4], end[5], end[6]};
    VecD vel4_end = m_st.velocity(pos4_end, vel3_end, 0.);

    double freq_init = g_init.prod(m_vel4).dot(vel4_init);

    VecD vel4_stars = {1., 0., 0., 0.};
    double freq_end = g_end.prod(vel4_stars).dot(vel4_end);

    // std::cout << std::endl;
    // std::cout << "FREQ INIT = " << freq_init << std::endl;
    // std::cout << "FREQ END  = " << freq_end << std::endl;

    return freq_end / freq_init;
}