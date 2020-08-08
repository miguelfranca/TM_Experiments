
#include "Spacetime.hpp"

#include <cassert>
#include <cmath>

VecD Spacetime::velocity(VecD pos4, VecD vel3, double V) const
{
    auto g = metric(pos4);

    // a * v0^2 + b * v0 + c = V
    double a = g[0][0];

    double b = 0.;
    FOR(i, 3) { b += g[0][i + 1] * vel3[i] + g[i + 1][0] * vel3[i]; }

    double c = -V;
    FOR(i, j, 3, 3) { c += g[i + 1][j + 1] * vel3[i] * vel3[j]; }

    double v0;
    if (a == 0)
    {
        if (b == 0)
        {
            throw std::runtime_error("[Spacetime::velocity] Something is "
                                     "wrong with your metric... ");
            return VecD();
        }
        v0 = -c / b;
    }
    else
    {
        v0 =
            // (-b + (a < 0 ? -1. : 1.) * sqrt(b * b - 4. * a * c)) / (2. *
            // a);
            (-b - sqrt(b * b - 4. * a * c)) / (2. * a);
    }

    // VecD vel4 = {v0, vel3[0], vel3[1], vel3[2]};
    // vel4.print();
    // return vel4;
    return {v0, vel3[0], vel3[1], vel3[2]};
}

MatrixD Flat::metric(const VecD &pos) const
{
    double r = pos[1];
    double sintheta = sin(pos[2]);

    MatrixD g(4, 4);
    g[0][0] = -1.;
    g[0][1] = 1;
    g[1][0] = g[0][1];
    g[2][2] = r * r;
    g[3][3] = g[2][2] * sintheta * sintheta;

    return g;
}

MatrixD Flat::imetric(const VecD &pos) const
{
    double r = pos[1];
    double sintheta = sin(pos[2]);

    MatrixD g(4, 4);
    g[0][1] = 1;
    g[1][0] = g[0][1];
    g[1][1] = 1.;
    g[2][2] = 1. / (r * r);
    g[3][3] = g[2][2] / (sintheta * sintheta);

    return g;
}

Vec<MatrixD> Flat::christoffels(const VecD &pos) const
{
    Vec<MatrixD> chris(4, MatrixD(4, 4, 0.));

    double r = pos[1];

    double theta = pos[2];
    double sintheta = sin(theta);
    double sintheta2 = sintheta * sintheta;
    double costheta = cos(theta);

    // t
    chris[0][2][2] = -r;
    chris[0][3][3] = chris[0][2][2] * sintheta2;

    // r
    chris[1][2][2] = -r;
    chris[1][3][3] = chris[1][2][2] * sintheta2;

    // theta
    chris[2][1][2] = 1. / r;
    chris[2][2][1] = chris[2][1][2];
    chris[2][3][3] = -costheta * sintheta;

    // phi
    chris[3][1][3] = 1. / r;
    chris[3][3][1] = chris[3][1][3];
    chris[3][2][3] = costheta / sintheta;
    chris[3][3][2] = chris[3][2][3];

    return chris;
}

Schwarzschild::Schwarzschild(double a_M) : M(a_M) {}

MatrixD Schwarzschild::metric(const VecD &pos) const
{
    double r = pos[1];
    double sintheta = sin(pos[2]);

    MatrixD g(4, 4);
    g[0][0] = -f(r);
    g[0][1] = 1;
    g[1][0] = g[0][1];
    g[2][2] = r * r;
    g[3][3] = g[2][2] * sintheta * sintheta;

    return g;
}

MatrixD Schwarzschild::imetric(const VecD &pos) const
{
    double r = pos[1];
    double sintheta = sin(pos[2]);

    MatrixD g(4, 4);
    g[0][1] = 1;
    g[1][0] = g[0][1];
    g[1][1] = f(r);
    g[2][2] = 1. / (r * r);
    g[3][3] = g[2][2] / (sintheta * sintheta);

    return g;
}
Vec<MatrixD> Schwarzschild::christoffels(const VecD &pos) const
{
    Vec<MatrixD> chris(4, MatrixD(4, 4, 0.));

    double r = pos[1];
    double r2 = r * r;

    double theta = pos[2];
    double sintheta = sin(theta);
    double sintheta2 = sintheta * sintheta;
    double costheta = cos(theta);

    // t
    chris[0][0][0] = M / r2;
    chris[0][2][2] = -r;
    chris[0][3][3] = chris[0][2][2] * sintheta2;

    // r
    chris[1][0][0] = M * f(r) / (r2);
    chris[1][0][1] = -M / r2;
    chris[1][1][0] = chris[1][0][1];
    chris[1][2][2] = 2. * M - r;
    chris[1][3][3] = chris[1][2][2] * sintheta2;

    // theta
    chris[2][1][2] = 1. / r;
    chris[2][2][1] = chris[2][1][2];
    chris[2][3][3] = -costheta * sintheta;

    // phi
    chris[3][1][3] = 1. / r;
    chris[3][3][1] = chris[3][1][3];
    chris[3][2][3] = costheta / sintheta;
    chris[3][3][2] = chris[3][2][3];

    return chris;
}

double
Schwarzschild::BH_radius() const // informs of the radius of an existing BH
{
    return 2. * M;
}

Kerr::Kerr(double a_M, double a_a) : M(a_M), a(a_a), a2(a_a * a_a)
{
    assert(abs(a) <= M);
}

MatrixD Kerr::metric(const VecD &pos) const
{
    double r = pos[1];
    double r2 = r * r;
    double sintheta = sin(pos[2]);
    double sintheta2 = sintheta * sintheta;
    double costheta = cos(pos[2]);
    double costheta2 = costheta * costheta;

    double sig = sigma(r2, costheta2);
    double fk = f(r, sig);
    double fkm1 = fk - 1.;

    MatrixD g(4, 4);
    g[0][0] = -fk;
    g[0][1] = 1;
    g[1][0] = g[0][1];
    g[0][3] = a * sintheta2 * fkm1;
    g[3][0] = g[0][3];
    g[1][3] = -a * sintheta2;
    g[3][1] = g[1][3];
    g[2][2] = sig;
    g[3][3] = sintheta2 * (r2 + a2 - a2 * sintheta2 * fkm1);

    return g;
}

MatrixD Kerr::imetric(const VecD &pos) const
{
    double r = pos[1];
    double r2 = r * r;
    double sintheta = sin(pos[2]);
    double sintheta2 = sintheta * sintheta;
    double costheta = cos(pos[2]);
    double costheta2 = costheta * costheta;

    double sig = sigma(r2, costheta2);
    double delta = a2 + r2 - 2. * M * r;

    MatrixD ig(4, 4);
    ig[0][0] = a2 * sintheta2 / sig;
    ig[0][1] = (a2 + r2) / sig;
    ig[1][0] = ig[0][1];
    ig[1][1] = delta / sig;
    ig[0][3] = a / sig;
    ig[3][0] = ig[0][3];
    ig[1][3] = a / sig;
    ig[3][1] = ig[1][3];
    ig[2][2] = 1. / sig;
    ig[3][3] = 1. / (sintheta2 * sig);

    return ig;
}
Vec<MatrixD> Kerr::christoffels(const VecD &pos) const
{
    Vec<MatrixD> chris(4, MatrixD(4, 4, 0.));

    double r = pos[1];
    double r2 = r * r;
    double sintheta = sin(pos[2]);
    double sintheta2 = sintheta * sintheta;
    double costheta = cos(pos[2]);
    double costheta2 = costheta * costheta;
    double sin2theta = 2. * sintheta * costheta;
    double cottheta = costheta / sintheta;

    double sig = sigma(r2, costheta2);
    double sig2 = sig * sig;
    double sig3 = sig2 * sig;
    double sigm = r2 - a2 * costheta2;
    double r2a2 = r2 + a2;
    double delta = a2 + r2 - 2. * M * r;

    // t
    chris[0][0][0] = M * r2a2 * sigm / sig3;
    chris[0][0][2] = -a2 * M * r * sin2theta / sig2;
    chris[0][2][0] = chris[0][0][2];
    chris[0][0][3] = -a * M * r2a2 * sigm * sintheta2 / sig3;
    chris[0][3][0] = chris[0][0][3];
    chris[0][1][2] = -0.5 * sin2theta * a2 / sig;
    chris[0][2][1] = chris[0][1][2];
    chris[0][1][3] = a * r * sintheta2 / sig;
    chris[0][3][1] = chris[0][1][3];
    chris[0][2][2] = -r * r2a2 / sig;
    chris[0][2][3] = sin2theta * sintheta2 * a2 * a * M * r / sig2;
    chris[0][3][2] = chris[0][2][3];
    chris[0][3][3] =
        r2a2 * sintheta2 * (-r * sig2 + a2 * M * sigm * sintheta2) / sig3;

    // r
    chris[1][0][0] = M * delta * sigm / sig3;
    chris[1][0][1] = -M * sigm / sig2;
    chris[1][1][0] = chris[1][0][1];
    chris[1][0][3] = -a * M * delta * sigm * sintheta2 / sig3;
    chris[1][3][0] = chris[1][0][3];
    chris[1][1][2] = -0.5 * sin2theta * a2 / sig;
    chris[1][2][1] = chris[1][1][2];
    chris[1][1][3] =
        a * (r2 * (r + M) + a2 * (r - M) * costheta2) * sintheta2 / sig2;
    chris[1][3][1] = chris[1][1][3];
    chris[1][2][2] = -r * delta / sig;
    chris[1][3][3] =
        -delta * sintheta2 * (r * sig2 - a2 * M * sigm * sintheta2) / sig3;

    // theta
    chris[2][0][0] = -sin2theta * a2 * M * r / sig3;
    chris[2][0][3] = a * M * r * r2a2 * sin2theta / sig3;
    chris[2][3][0] = chris[2][0][3];
    chris[2][1][2] = r / sig;
    chris[2][2][1] = chris[2][1][2];
    chris[2][1][3] = 0.5 * sin2theta * a / sig;
    chris[2][3][1] = chris[2][1][3];
    chris[2][2][2] = -0.5 * sin2theta * a2 / sig;
    chris[2][3][3] = 0.5 * sin2theta *
                     (-r2a2 * sig2 - 4. * a2 * M * r * sig * sintheta2 -
                      2. * a2 * a2 * M * r * sintheta2 * sintheta2) /
                     sig3;

    // phi
    chris[3][0][0] = a * M * sigm / sig3;
    chris[3][0][2] = -2. * a * M * r * cottheta / sig2;
    chris[3][2][0] = chris[3][0][2];
    chris[3][0][3] = -a2 * M * sigm * sintheta2 / sig3;
    chris[3][3][0] = chris[3][0][3];
    chris[3][1][2] = -a * cottheta / sig;
    chris[3][2][1] = chris[3][1][2];
    chris[3][1][3] = r / sig;
    chris[3][3][1] = chris[3][1][3];
    chris[3][2][2] = -a * r / sig;
    chris[3][2][3] = cottheta + a2 * M * r * sin2theta / sig2;
    chris[3][3][2] = chris[3][2][3];
    chris[3][3][3] = -a * r * sintheta2 / sig +
                     a2 * a * M * sigm * sintheta2 * sintheta2 / sig3;

    return chris;
}

double Kerr::BH_radius() const // informs of the radius of an existing BH
{
    return M + sqrt(M * M - a2);
}
