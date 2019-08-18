#include "UserPendulum.h"

VecD UserPendulum::invertedPendulumRHS(const VecD& x, const VecD& params) {
	double A = params[0];

	return VecD({ 1,
		x[3],
		x[4],
		((m * g * sin(x[1]) * cos(x[1]) - m * L * x[3] * x[3] * sin(x[1]) + A * (M+m) ) / (M + m * sin(x[1]) * sin(x[1])) * cos(x[1]) + g * sin(x[1])) / L,
		(m * g * sin(x[1]) * cos(x[1]) - m * L * x[3] * x[3] * sin(x[1]) + A * (M+m) ) / (M + m * sin(x[1]) * sin(x[1]))
	});
}

void UserPendulum::restart(){
	x0[0] = 0.;
	x0[1] = 10*toRAD;
	x0[2] = 0.;
	x0[3] = 0.;
	x0[4] = 0.;
	// 	x0[1] = Rand(-thetaMAX, thetaMAX) * 0.5;
	// 	x0[2] = Rand(-xMAX, xMAX) * 0.95; //make sure it starts inside the track
	// 	x0[3] = Rand(-thetaDMAX, thetaDMAX);
	// 	x0[4] = Rand(-xDMAX, xDMAX) * 0.;
}


UserPendulum::UserPendulum(sf::RenderTarget* renderer) : 
		base(sf::Vector2f(300 * SW, 100 * SH), CENTER_WINDOW + sf::Vector2f(0, 200 * SH), GF::CENTERED), // setup base car
		line(sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH)), sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L)))), // setup line
		circle(15, sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L)))), // setup circle

		eq(UserPendulum::invertedPendulumRHS, 4),
		x0(5),

m_lenght(toPixels(L)) // lenght of pendulum
{
	base_tex.loadFromFile("res/textures/truck.jpg");
	base.setTexture(&base_tex);
	m_target = renderer;

	restart();
	move(x0[2],x0[1]);
}

bool UserPendulum::handleEvent(GF::Event& event)
{
	static GF::ToggleKey SPACE(sf::Keyboard::Space);
	if (SPACE.isKeyPressedOnce(event))
		restart();

	return true;
}

bool UserPendulum::update(const float fElapsedTime, const float fTotalTime)
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

	move(x0[2],x0[1]);

	return true;
}

bool UserPendulum::draw()
{
	m_target->draw(base);
	m_target->draw(line);
	m_target->draw(circle);
	float minX = std::max(base.getGlobalBounds().width / 2., toPixels(L) - base.getGlobalBounds().width / 4.);
	float maxX = SCREENWIDTH - std::max(base.getGlobalBounds().width / 2., toPixels(L) + base.getGlobalBounds().width / 4.);

	int sign = (base.getPosition().x <= minX) ? 1 : -1;

	if(base.getPosition().x <= minX || base.getPosition().x >= maxX){
		if(base.getPosition().x <= 0. || base.getPosition().x >= SCREENWIDTH)
			x0[2] += sign*2*xMAX;

		base.setPosition((int)SCREENWIDTH*sign + base.getPosition().x, base.getPosition().y);
		line.line[0].position.x = (int)SCREENWIDTH*sign + line.line[0].position.x;
		line.line[1].position.x = (int)SCREENWIDTH*sign + line.line[1].position.x;
		circle.setPosition((int)SCREENWIDTH*sign + circle.getPosition().x, circle.getPosition().y);
		m_target->draw(base);
		m_target->draw(line);
		m_target->draw(circle);
	}

	return true;
}

void UserPendulum::move(float pos, float angle) {
	pos = toPixels(pos+xMAX);

	base.setPosition(pos, base.getPosition().y);
	line.line[0].position.x = pos + base.getGlobalBounds().width / 4;
	line.line[1].position.x = line.line[0].position.x - m_lenght * sin(angle);
	line.line[1].position.y = line.line[0].position.y - m_lenght * cos(angle);

	circle.setPosition(line.line[1].position);
}