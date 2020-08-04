#include "Application.h"

#include "Geodesic.hpp"
#include "Particle.hpp"
#include "Spacetime.hpp"

#include "Clock.hpp"
#include "ODEsolver.hpp"

Application::Application(std::string t)
{
    title = t;
    setStaticScreen(true);
    setupWindow(1000, 1000, 1000, 0);
}

// called once before the loop starts
bool Application::onCreate()
{
    // Flat st;

    double M = 0.25;
    Schwarzschild st(M);

    // double M = 0.25;
    // double a = 0.;
    // Kerr st(M, a);

    VecD position({10., M_PI / 2., 0});
    double alpha = 0. / 180. * M_PI;
    double beta = 180. / 180. * M_PI;
    double modV = 1.;

    Particle par(st, position, alpha, beta, modV, -1.);
    par.setAngleViews(45. / 180. * M_PI, 45. / 180. * M_PI);
    auto mat = par.view(1000);
    // mat.print();

    // sky.loadFromFile("res/images/HUBBLE_cut_3.png");
    // sky.loadFromFile("res/images/stars_2.png");
    sky.loadFromFile("res/images/stars_3.jpg");
    sf::Image view;
    view.create(mat.getNC(), mat.getNL());

    // bool do_elliptic_parametrization = true;
    bool do_elliptic_parametrization = false;

    for (unsigned l = 0; l < mat.getNL(); ++l)
    {
        for (unsigned c = 0; c < mat.getNC(); ++c)
        {
            auto vec = mat[l][c];
            sf::Color color;
            double r = vec[1];

            if (r < st.BH_radius() || std::isnan(r))
            {
                color = BLACK;
            }
            else
            {
                double theta = vec[2];
                double phi = vec[3];

                theta -= M_PI / 2;

                while (phi > M_PI)
                    phi -= 2. * M_PI;

                while (phi <= -M_PI)
                    phi += 2. * M_PI;

                double x, y;
                if (do_elliptic_parametrization)
                {

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
                else
                {
                    x = phi;
                    y = theta;
                }

                sf::Vector2u size = sky.getSize();
                y = (y + M_PI / 2.) / M_PI * size.y;
                x = (x + M_PI) / (2. * M_PI) * size.x;

                // flip y because SFML grows y downwards
                y = size.y - y;

                color = sky.getPixel(std::round(x), std::round(y));
                auto lambda_color = [](unsigned color) {
                    unsigned new_color = color + 5;
                    return std::min(new_color, (unsigned)255);
                };
                color = sf::Color(lambda_color(color.r), lambda_color(color.g),
                                  lambda_color(color.b));
            }

            view.setPixel(c, l, color);
        }
    }

    std::cout << "FINISHED" << std::endl;

    sf::Texture txt;

    txt.loadFromImage(view);
    sf::Sprite sprt;

    sprt.setTexture(txt, true);
    sprt.setPosition(0, 0);

    double sx = ((double)SCREENWIDTH) / mat.getNC();
    double sy = ((double)SCREENHEIGHT) / mat.getNL();

    sx = std::min(sx, sy);
    sprt.scale(sx, sx);

    window.draw(sprt);

    view.saveToFile("screenshot" + std::to_string(time(NULL)) + ".png");

    return true;
}

// first thing to be called every frame
bool Application::onHandleEvent(GF::Event &event) { return true; }

// called every frame before draw
bool Application::onUpdate(const float fElapsedTime, const float fTotalTime)
{
    return true;
}

// last thing to be called every frame
bool Application::onDraw() { return true; }

// called before exiting the app
void Application::onDestroy() {}

void Application::onSwitch(std::string other) {}
