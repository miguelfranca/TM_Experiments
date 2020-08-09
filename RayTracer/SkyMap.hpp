#pragma once

#include <GraphicsFramework/sfml.h>

#include "Spacetime.hpp"
#include "Matrix.hpp"
#include "Tools.hpp"


class SkyMap
{
public:
    SkyMap(const Spacetime& a_st, bool do_elliptic_parametrization);

    virtual sf::Color getPixelColor(double x, double y) = 0;

    sf::Image getSkyView(const Matrix<VecD>& mat);

    VecD getCoordinates(double theta, double phi);

protected:
    const Spacetime& st;
    bool do_elliptic_parametrization = false;
};

class ColorSkyMap : public SkyMap
{
public:
    ColorSkyMap(const Spacetime& a_st, bool do_elliptic_parametrization, double grid_angle,
                double thickness, double color_ring = 0.);

    sf::Color getPixelColor(double x, double y);

private:
    double grid_angle;
    double thickness;
    double color_ring;
};

class ImageSkyMap : public SkyMap
{
public:
    ImageSkyMap(const Spacetime& a_st, const std::string& filename, bool do_elliptic_parametrization);
    sf::Color getPixelColor(double x, double y);

private:
    sf::Image sky;
};

sf::Color redshiftColor(sf::Color color, double factor);
sf::Color wavelengthToColor(double wavelength);
