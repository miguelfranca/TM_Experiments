
#include "Spacetime.hpp"

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
    return {v0, vel3[0], vel3[1], vel3[2]};
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
