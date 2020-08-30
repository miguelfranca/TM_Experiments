#include "SkyMap.hpp"
#include "Tools.hpp"

#include <cassert>

SkyMap::SkyMap(const Spacetime& a_st, bool ep)
	: st(a_st), do_elliptic_parametrization(ep)
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
		    std::sqrt(theta2 * (PI2 - phi2) / (PI2 * PI2 - 4. * theta2 * phi2));
	}
	else {
		x = phi;
		y = theta;
	}

	return {x, y};
}

// sf::Image SkyMap::getSkyView(const Matrix<VecD>& mat)
sf::Image SkyMap::getSkyView(Kokkos::View<double*** >::HostMirror mat)
{
	unsigned nl = mat.extent_int(0);
	unsigned nc = mat.extent_int(1);
	// mat.getNC()
	// mat.getNL()

	sf::Image view;
	view.create(nc, nl);

	for (unsigned c = 0; c < nc; ++c) {
		for (unsigned l = 0; l < nl; ++l) {
			// auto vec = mat[l][c];
			sf::Color color;
			// double r = vec[1];
			double r = mat(l,c,0);

			if (r < st.BH_radius() || std::isnan(r)) {
				color = BLACK;
			}
			else {
				// double theta = vec[2];
				// double phi = vec[3];
				double theta = mat(l,c,1);
				double phi = mat(l,c,2);

				VecD coords = getCoordinates(theta, phi);
				color = getPixelColor(coords[0], coords[1]);
				color = redshiftColor(color, mat(l,c,3));
				// color = redshiftColor(color, vec[7]);
			}

			view.setPixel(c, l, color);
		}
	}

	return view;
}

ColorSkyMap::ColorSkyMap(const Spacetime& a_st,
                         bool do_elliptic_parametrization, double a_grid_angle,
                         double a_thickness, double a_color_ring)
	: SkyMap(a_st, do_elliptic_parametrization), grid_angle(a_grid_angle),
	  thickness(a_thickness), color_ring(a_color_ring)
{
}

sf::Color ColorSkyMap::getPixelColor(double x, double y)
{
	sf::Color color;

	static double pid2 = M_PI / 2.;

	if (x > 0 && y > 0) {
		if (x < pid2)
			color = sf::Color(204, 0, 0); // dark red
		else
			color = sf::Color(255, 102, 102); // light red
	}
	else if (x < 0 && y > 0) {
		if (x > -pid2)
			color = sf::Color(255, 153, 51); // light orange
		else
			color = sf::Color(255, 255, 51); // yellow
	}
	else if (x > 0 && y < 0) {
		if (x < pid2)
			color = sf::Color(0, 0, 255); // dark blue
		else
			color = sf::Color(102, 178, 255); // light blue
	}
	else if (x < 0 && y < 0) {
		if (x > -pid2)
			color = sf::Color(0, 153, 0); // dark green
		else
			color = sf::Color(102, 255, 102); // light green
	}

	if (std::abs(std::remainder(x, grid_angle)) <= thickness)
		color = BLACK;

	if (std::abs(std::remainder(y, grid_angle)) <= thickness)
		color = BLACK;

	// spot of light
	double r = sqrt(x * x + y * y);

	if (r <= color_ring) {
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
                         bool do_elliptic_parametrization)
	: SkyMap(a_st, do_elliptic_parametrization)
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

sf::Color redshiftColor(sf::Color color, double factor)
{
	static double red = 700.;
	static double green = 510.;
	static double blue = 440.;

	sf::Color red_new = wavelengthToColor(red * factor);
	sf::Color green_new = wavelengthToColor(green * factor);
	sf::Color blue_new = wavelengthToColor(blue * factor);

	double R, G, B;
	R = color.r / 255.;
	G = color.g / 255.;
	B = color.b / 255.;

	assert(color.a == 255);

	sf::Color out = sf::Color(std::min(255., red_new.r * R + green_new.r * G + blue_new.r * B),
	                          std::min(255., red_new.g * R + green_new.g * G + blue_new.g * B),
	                          std::min(255., red_new.b * R + green_new.b * G + blue_new.b * B),
	                          255.);
	return out;
}



sf::Color wavelengthToColor(double wavelength)
{
	double R, G, B, A;

	if (wavelength >= 380. && wavelength < 440.) {
		R = -1 * (wavelength - 440.) / (440. - 380.);
		G = 0.;
		B = 1.;
	}
	else if (wavelength >= 440. && wavelength < 490.) {
		R = 0.;
		G = (wavelength - 440.) / (490. - 440.);
		B = 1.;
	}
	else if (wavelength >= 490. && wavelength < 510.) {
		R = 0.;
		G = 1.;
		B = -1 * (wavelength - 510.) / (510. - 490.);
	}
	else if (wavelength >= 510. && wavelength < 580.) {
		R = (wavelength - 510.) / (580. - 510.);
		G = 1.;
		B = 0.;
	}
	else if (wavelength >= 580. && wavelength < 645.) {
		R = 1.;
		G = -1 * (wavelength - 645.) / (645. - 580.);
		B = 0.;
	}
	else if (wavelength >= 645. && wavelength <= 780.) {
		R = 1.;
		G = 0.;
		B = 0.;
	}
	else {
		R = 0.;
		G = 0.;
		B = 0.;
	}

	// intensty is lower at the edges of the visible spectrum.
	if (wavelength > 780. || wavelength < 380.) {
		A = 0.;
	}
	else if (wavelength > 700.) {
		A = (780. - wavelength) / (780. - 700.);
	}
	else if (wavelength < 420.) {
		A = (wavelength - 380.) / (420. - 380.);
	}
	else {
		A = 1.;
	}

	return sf::Color(R * 255 * A, G * 255 * A, B * 255 * A, 255);
}

