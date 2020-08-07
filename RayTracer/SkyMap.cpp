#include "SkyMap.hpp"
#include "Tools.hpp"

SkyMap::SkyMap(const Spacetime& a_st, bool ep) : st(a_st),
	do_elliptic_parametrization(ep)
{

}

VecD SkyMap::getCoordinates(double theta, double phi)
{
	theta -= M_PI / 2;

	while (phi > M_PI)
		phi -= 2. * M_PI;

	while (phi <= -M_PI)
		phi += 2. * M_PI;

	double x, y;

	if (do_elliptic_parametrization) {

		// code for elliptic parametrization of sky map
		double phi2 = phi * phi;
		double PI2 = M_PI * M_PI;
		double theta2 = theta * theta;

		x = (phi < 0 ? -1 : 1) * M_PI *
		    std::sqrt(phi2 * (PI2 - 4. * theta2) /
		              (PI2 * PI2 - 4. * theta2 * phi2));
		y = (theta < 0 ? -1 : 1) * M_PI *
		    std::sqrt(theta2 * (PI2 - phi2) /
		              (PI2 * PI2 - 4. * theta2 * phi2));
	}
	else {
		x = phi;
		y = theta;
	}

	return {x, y};
}

sf::Image SkyMap::getSkyView(const Matrix<VecD>& mat)
{
	sf::Image view;
	view.create(mat.getNC(), mat.getNL());

	for (unsigned l = 0; l < mat.getNL(); ++l) {
		for (unsigned c = 0; c < mat.getNC(); ++c) {
			auto vec = mat[l][c];
			sf::Color color;
			double r = vec[1];

			if (r < st.BH_radius() || std::isnan(r)) {
				color = BLACK;
			}
			else {
				double theta = vec[2];
				double phi = vec[3];

				VecD coords = getCoordinates(theta, phi);
				color = getPixelColor(coords[0], coords[1]);

			}

			view.setPixel(c, l, color);
		}
	}

	return view;
}

ColorSkyMap::ColorSkyMap(const Spacetime& a_st, bool do_elliptic_parametrization,
                         double a_grid_angle, double a_thickness, double a_color_ring) : SkyMap(a_st, do_elliptic_parametrization),
	grid_angle(a_grid_angle),
	thickness(a_thickness), color_ring(a_color_ring) {}

sf::Color ColorSkyMap::getPixelColor(double x, double y)
{
	sf::Color color;

	if (x > 0 && y > 0)
		color = RED;
	else if (x < 0 && y > 0)
		color = YELLOW;
	else if (x > 0 && y < 0)
		color = BLUE;
	else if (x < 0 && y < 0)
		color = GREEN;

	if (std::abs(std::remainder(x, grid_angle)) <= thickness)
		color = BLACK;

	if (std::abs(std::remainder(y, grid_angle)) <= thickness)
		color = BLACK;

	// spot of light
	double r = sqrt(x*x+y*y);
	if(r<=color_ring){
		auto color_2 = WHITE;
		double factor = (color_ring - r) / color_ring;
		factor = pow(factor, 0.3);
		color.r = (1. - factor) * color.r + factor * color_2.r;
		color.g = (1. - factor) * color.g + factor * color_2.g;
		color.b = (1. - factor) * color.b + factor * color_2.b;
		color.a = (1. - factor) * color.a + factor * color_2.a;
	}

	return color;
}

ImageSkyMap::ImageSkyMap(const Spacetime& a_st, const std::string& filename,
                         bool do_elliptic_parametrization) : SkyMap(a_st, do_elliptic_parametrization)
{
	if (!fileExists(filename))
		errorMsg("Could not load file " + filename);

	sky.loadFromFile(filename);
}

sf::Color ImageSkyMap::getPixelColor(double x, double y)
{
	sf::Vector2u size = sky.getSize();
	y = (y + M_PI / 2.) / M_PI * size.y;
	x = (x + M_PI) / (2. * M_PI) * size.x;

	// flip y because SFML grows y downwards
	y = size.y - y;

	sf::Color color;
	color = sky.getPixel(std::round(x), std::round(y));

	int offset = 5;

	color = sf::Color(std::min((unsigned)(color.r + offset), (unsigned)(255)),
	                  std::min((unsigned)(color.g + offset), (unsigned)(255)),
	                  std::min((unsigned)(color.b + offset), (unsigned)(255)));

	return color;
}
