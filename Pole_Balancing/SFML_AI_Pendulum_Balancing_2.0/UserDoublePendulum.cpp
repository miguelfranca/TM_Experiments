#include "UserDoublePendulum.h"

#define toRAD (M_PI / 180.)
#define g 3
// 9.8
#define M 1000.0
#define m1 1.1
#define m2 0.001
#define L1 1.0
#define L2 0.5
#define thetaMAX (36. * toRAD)
#define xMAX 2.5
#define thetaDMAX 1.5
#define xDMAX 1.0
#define A_MAGN 3.0

double UserDoublePendulum::toPixels(double l)
{
    return l / (2. * xMAX) * SCREENWIDTH;
}
double UserDoublePendulum::toMeters(double p)
{
    return p * (2. * xMAX) / SCREENWIDTH;
}

VecD UserDoublePendulum::invertedPendulumRHS(const VecD &vars,
                                             const VecD &params)
{
    // return VecD({1, 0, 0, 0, 0, 0, 0});
    double q1 = vars[1];
    double q2 = vars[2];
    // double x = vars[3];
    double q1p = vars[4];
    double q2p = vars[5];
    double xp = vars[6];

    double A = params[0];
    double d1 = params[1];
    double d2 = params[2];
    double d3 = params[3];

    // acceleration is only for the car, whether the pendulums are light or
    // heavy
    double F1 = A * M - d1 * xp;
    double F2 = -d2 * q1p;
    double F3 = -d3 * q2p;

    double term_ms = 2. * m1 + m2 - m2 * cos(2. * (q1 - q2));

    double xpp_den =
        L1 * L2 * (M * term_ms + 2. * m1 * (m1 + m2) * sin(q1) * sin(q1));

    double term_aux_2 = (-(2. * m1 + m2) * cos(q1) + m2 * cos(q1 - 2. * q2));

    double xpp =
        F1 * L1 * L2 * term_ms + F2 * L2 * term_aux_2 +
        F3 * (-2. * L1 * (m1 + m2) * sin(q1) * sin(q1 - q2)) -
        g * L1 * L2 * m1 * (m1 + m2) * sin(2. * q1) +
        2. * L1 * L2 * m1 * sin(q1) *
            (L1 * (m1 + m2) * q1p * q1p + L2 * m2 * cos(q1 - q2) * q2p * q2p);
    xpp /= xpp_den;

    double q1_den = L1 * term_ms;

    double q1pp = 2. * F2 / L1 + F3 * (-2. * cos(q1 - q2)) / L2 +
                  xpp * term_aux_2 +
                  2. * g * (m1 * sin(q1) + m2 * cos(q2) * sin(q1 - q2)) -
                  2. * m2 * sin(q1 - q2) *
                      (L1 * cos(q1 - q2) * q1p * q1p + L2 * q2p * q2p);
    q1pp /= q1_den;

    double q2pp = F3 / (L2 * L2 * m2) - xpp * cos(q2) / L2 -
                  L1 * q1pp * cos(q1 - q2) / L2 + g * sin(q2) / L2 +
                  L1 * sin(q1 - q2) * q1p * q1p / L2;

    return VecD({1, q1p, q2p, xp, q1pp, q2pp, xpp});
}

// variables explanation
// x[0] = t
// x[1] = theta1
// x[2] = theta2
// x[3] = x
// x[4] = theta1.
// x[5] = theta2.
// x[6] = x.
void UserDoublePendulum::restart()
{
    x0[0] = 0.;
    x0[1] = -1 * toRAD;
    x0[2] = -1 * toRAD;
    x0[3] = 0.;
    x0[4] = 0.;
    x0[5] = 0.;
    x0[6] = 0.;
    // 	x0[1] = Rand(-thetaMAX, thetaMAX) * 0.5;
    // 	x0[2] = Rand(-xMAX, xMAX) * 0.95; //make sure it starts inside the track
    // 	x0[3] = Rand(-thetaDMAX, thetaDMAX);
    // 	x0[4] = Rand(-xDMAX, xDMAX) * 0.;
}

UserDoublePendulum::UserDoublePendulum(sf::RenderTarget *renderer)
    : base(sf::Vector2f(300 * SW, 100 * SH),
           CENTER_WINDOW + sf::Vector2f(0, 200 * SH),
           GF::CENTERED), // setup base car
      line1(sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH)),
            sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) -
                         sf::Vector2f(0, toPixels(L1)))), // setup line
      line2(sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH)) -
                sf::Vector2f(0, toPixels(L1)),
            sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) -
                         sf::Vector2f(0, toPixels(L2 + L1)))), // setup line
      circle1(15, sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) -
                               sf::Vector2f(0, toPixels(L1)))), // setup circle
      circle2(15,
              sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) -
                           sf::Vector2f(0, toPixels(L2 + L1)))), // setup circle

      eq(UserDoublePendulum::invertedPendulumRHS, 4), x0(7),

      m_lenght1(toPixels(L1)), // lenght of pendulum1
      m_lenght2(toPixels(L2))  // lenght of pendulum2
{
    base_tex.loadFromFile("res/textures/truck.jpg");
    base.setTexture(&base_tex);
    m_target = renderer;

    restart();
    move(x0[3], x0[2], x0[1]);
}

bool UserDoublePendulum::handleEvent(GF::Event &event)
{
    static GF::ToggleKey SPACE(sf::Keyboard::Space);
    if (SPACE.isKeyPressedOnce(event))
        restart();

    return true;
}

bool UserDoublePendulum::update(const float fElapsedTime,
                                const float fTotalTime)
{
    static GF::ToggleKey LEFT(sf::Keyboard::Left);
    static GF::ToggleKey RIGHT(sf::Keyboard::Right);

    float A = 0.;
    if (LEFT.isKeyPressed())
        A = -A_MAGN;
    if (RIGHT.isKeyPressed())
        A = A_MAGN;

    eq.setParams({A, 0., 0., 0.});
    x0 = eq.stepRK4(x0, fElapsedTime);

    move(x0[3], x0[2], x0[1]);

    return true;
}

bool UserDoublePendulum::draw()
{
    m_target->draw(base);
    m_target->draw(line1);
    m_target->draw(circle1);

    m_target->draw(line2);
    m_target->draw(circle2);

    float L = std::max(L1, L2);
    float minX = std::max(base.getGlobalBounds().width / 2.,
                          toPixels(L) - base.getGlobalBounds().width / 4.);
    float maxX =
        SCREENWIDTH - std::max(base.getGlobalBounds().width / 2.,
                               toPixels(L) + base.getGlobalBounds().width / 4.);

    int sign = (base.getPosition().x <= minX) ? 1 : -1;

    if (base.getPosition().x <= minX || base.getPosition().x >= maxX)
    {
        if (base.getPosition().x <= 0. || base.getPosition().x >= SCREENWIDTH)
            x0[3] += sign * 2 * xMAX;

        base.setPosition((int)SCREENWIDTH * sign + base.getPosition().x,
                         base.getPosition().y);
        line1.line[0].position.x =
            (int)SCREENWIDTH * sign + line1.line[0].position.x;
        line1.line[1].position.x =
            (int)SCREENWIDTH * sign + line1.line[1].position.x;
        circle1.setPosition((int)SCREENWIDTH * sign + circle1.getPosition().x,
                            circle1.getPosition().y);

        line2.line[0].position.x =
            (int)SCREENWIDTH * sign + line2.line[0].position.x;
        line2.line[1].position.x =
            (int)SCREENWIDTH * sign + line2.line[1].position.x;
        circle2.setPosition((int)SCREENWIDTH * sign + circle2.getPosition().x,
                            circle2.getPosition().y);

        m_target->draw(base);
        m_target->draw(line1);
        m_target->draw(line2);
        m_target->draw(circle1);
        m_target->draw(circle2);
    }

    return true;
}

void UserDoublePendulum::move(float pos, float angle2, float angle1)
{
    pos = toPixels(pos + xMAX);

    // angles are positive to the right (clock-wise)
    // (see pdf in folder)

    // pendulum 1
    base.setPosition(pos, base.getPosition().y);
    line1.line[0].position.x = pos + base.getGlobalBounds().width / 4;
    line1.line[1].position.x =
        line1.line[0].position.x + m_lenght1 * sin(angle1);
    line1.line[1].position.y =
        line1.line[0].position.y - m_lenght1 * cos(angle1);

    circle1.setPosition(line1.line[1].position);

    // pendulum 2
    base.setPosition(pos, base.getPosition().y);
    line2.line[0].position.x = line1.line[1].position.x;
    line2.line[0].position.y = line1.line[1].position.y;
    line2.line[1].position.x =
        line2.line[0].position.x + m_lenght2 * sin(angle2);
    line2.line[1].position.y =
        line2.line[0].position.y - m_lenght2 * cos(angle2);

    circle2.setPosition(line2.line[1].position);
}