#include "UserDoublePendulum.h"

VecD UserDoublePendulum::invertedPendulumRHS(const VecD& x, const VecD& params) {
	double A = params[0];

	double xpp = (m1*g*sin(x[1])*cos(x[1])-m1*L1*x[4]*x[4]*sin(x[1])
		+m2*g*sin(x[2])*cos(x[2])-m2*L2*x[5]*x[5]*sin(x[2])+ A * (M+m1+m2) )/(M+m1*sin(x[1])*sin(x[1])+m2*sin(x[2])*sin(x[2]));

	return VecD({1,
		x[4],
		x[5],
		x[6],
		(xpp*cos(x[1]) + g*sin(x[1]))/L1,
		(xpp*cos(x[2]) + g*sin(x[2]))/L2,
		xpp
	});
}

//variables explanation
//x[0] = t
//x[1] = theta1
//x[2] = theta2
//x[3] = x
//x[4] = theta1.
//x[5] = theta2.
//x[6] = x.
void UserDoublePendulum::restart(){
	x0[0] = 0.;
	x0[1] = 10*toRAD;
	x0[2] = 10*toRAD;
	x0[3] = 0.;
	x0[4] = 0.;
	x0[5] = 0.;
	x0[6] = 0.;
	// 	x0[1] = Rand(-thetaMAX, thetaMAX) * 0.5;
	// 	x0[2] = Rand(-xMAX, xMAX) * 0.95; //make sure it starts inside the track
	// 	x0[3] = Rand(-thetaDMAX, thetaDMAX);
	// 	x0[4] = Rand(-xDMAX, xDMAX) * 0.;
}


UserDoublePendulum::UserDoublePendulum(sf::RenderTarget* renderer) : 
		base(sf::Vector2f(300 * SW, 100 * SH), CENTER_WINDOW + sf::Vector2f(0, 200 * SH), GF::CENTERED), // setup base car
		line1(sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH)), sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L1)))), // setup line
		line2(sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH)), sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L2)))), // setup line
		circle1(15, sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L1)))), // setup circle
		circle2(15, sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L2)))), // setup circle

		eq(UserDoublePendulum::invertedPendulumRHS, 4),
		x0(7),

m_lenght1(toPixels(L1)), // lenght of pendulum1
m_lenght2(toPixels(L2)) // lenght of pendulum2
{
	base_tex.loadFromFile("res/textures/truck.jpg");
	base.setTexture(&base_tex);
	m_target = renderer;

	restart();
	move(x0[3], x0[2], x0[1]);
}

bool UserDoublePendulum::handleEvent(GF::Event& event)
{
	static GF::ToggleKey SPACE(sf::Keyboard::Space);
	if (SPACE.isKeyPressedOnce(event))
		restart();

	return true;
}

bool UserDoublePendulum::update(const float fElapsedTime, const float fTotalTime)
{
	static GF::ToggleKey LEFT(sf::Keyboard::Left);
	static GF::ToggleKey RIGHT(sf::Keyboard::Right);

	float A = 0.;
	if (LEFT.isKeyPressed())
		A = -A_MAGN;
	if (RIGHT.isKeyPressed())
		A = A_MAGN;

	eq.setParams({ A });
	x0 = eq.stepRK4(x0, fElapsedTime);

	move(x0[3],x0[2], x0[1]);

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
	float minX = std::max(base.getGlobalBounds().width / 2., toPixels(L) - base.getGlobalBounds().width / 4.);
	float maxX = SCREENWIDTH - std::max(base.getGlobalBounds().width / 2., toPixels(L) + base.getGlobalBounds().width / 4.);

	int sign = (base.getPosition().x <= minX) ? 1 : -1;

	if(base.getPosition().x <= minX || base.getPosition().x >= maxX){
		if(base.getPosition().x <= 0. || base.getPosition().x >= SCREENWIDTH)
			x0[3] += sign*2*xMAX;

		base.setPosition((int)SCREENWIDTH*sign + base.getPosition().x, base.getPosition().y);
		line1.line[0].position.x = (int)SCREENWIDTH*sign + line1.line[0].position.x;
		line1.line[1].position.x = (int)SCREENWIDTH*sign + line1.line[1].position.x;
		circle1.setPosition((int)SCREENWIDTH*sign + circle1.getPosition().x, circle1.getPosition().y);

		line2.line[0].position.x = (int)SCREENWIDTH*sign + line2.line[0].position.x;
		line2.line[1].position.x = (int)SCREENWIDTH*sign + line2.line[1].position.x;
		circle2.setPosition((int)SCREENWIDTH*sign + circle2.getPosition().x, circle2.getPosition().y);


		m_target->draw(base);
		m_target->draw(line1);
		m_target->draw(line2);
		m_target->draw(circle1);
		m_target->draw(circle2);
	}

	return true;
}

void UserDoublePendulum::move(float pos, float angle2, float angle1) {
	pos = toPixels(pos+xMAX);

	// pendulum 1
	base.setPosition(pos, base.getPosition().y);
	line1.line[0].position.x = pos + base.getGlobalBounds().width / 4;
	line1.line[1].position.x = line1.line[0].position.x - m_lenght1 * sin(angle1);
	line1.line[1].position.y = line1.line[0].position.y - m_lenght1 * cos(angle1);

	circle1.setPosition(line1.line[1].position);


	// pendulum 2
	base.setPosition(pos, base.getPosition().y);
	line2.line[0].position.x = pos + base.getGlobalBounds().width / 4;
	line2.line[1].position.x = line2.line[0].position.x - m_lenght2 * sin(angle2);
	line2.line[1].position.y = line2.line[0].position.y - m_lenght2 * cos(angle2);

	circle2.setPosition(line2.line[1].position);
}