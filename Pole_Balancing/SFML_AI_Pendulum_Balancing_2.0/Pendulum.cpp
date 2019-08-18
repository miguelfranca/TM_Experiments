#include "Pendulum.h"

double Pendulum::toPixels(double l){ return l/(2.*xMAX)*SCREENWIDTH; }
double Pendulum::toMeters(double p){ return p*(2.*xMAX)/SCREENWIDTH; }

VecD Pendulum::invertedPendulumRHS(const VecD& x, const VecD& params) {
	double A = params[0];

	return VecD({ 1,
		x[3],
		x[4],
		((m * g * sin(x[1]) * cos(x[1]) - m * L * x[3] * x[3] * sin(x[1]) + A * (M+m) ) / (M + m * sin(x[1]) * sin(x[1])) * cos(x[1]) + g * sin(x[1])) / L,
		(m * g * sin(x[1]) * cos(x[1]) - m * L * x[3] * x[3] * sin(x[1]) + A * (M+m) ) / (M + m * sin(x[1]) * sin(x[1]))
	});
}

VecD Pendulum::restart(){ return {0, 30.*toRAD, 0., 0., 0.}; }

void Pendulum::init(sf::RenderTarget* renderer){
	base_tex.loadFromFile("res/textures/truck.jpg");
	base.setTexture(&base_tex);
	m_target = renderer;

	x0 = restart();
	move(x0[2],x0[1]);
}


Pendulum::Pendulum(sf::RenderTarget* renderer, const std::string indv_net, const sf::Color color) : 
isdead(false),
	base(sf::Vector2f(300 * SW, 100 * SH), CENTER_WINDOW + sf::Vector2f(0, 200 * SH), GF::CENTERED, color), // setup base car
	line(sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH)), sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L))), color, color), // setup line
	circle(15, sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L))), color), // setup circle

	eq(Pendulum::invertedPendulumRHS, 4),
	x0(5),

m_lenght(toPixels(L)), // lenght of pendulum
net(indv_net)
{
	std::ifstream f(indv_net.c_str());
	if(f.good())
		net.read(indv_net);
	else 
		exit(1);

	init(renderer);
}



Pendulum::Pendulum(sf::RenderTarget* renderer, const NEAT::Network& indv_net, const sf::Color color) : 
isdead(false),
	base(sf::Vector2f(300 * SW, 100 * SH), CENTER_WINDOW + sf::Vector2f(0, 200 * SH), GF::CENTERED, color), // setup base car
	line(sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH)), sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L))), color, color), // setup line
	circle(15, sf::Vector2f(CENTER_WINDOW + sf::Vector2f(100, 180 * SH) - sf::Vector2f(0, toPixels(L))), color), // setup circle

	eq(Pendulum::invertedPendulumRHS, 4),
	x0(5),

m_lenght(toPixels(L)), // lenght of pendulum
net(indv_net)
{
	init(renderer);
}


bool Pendulum::handleEvent(GF::Event& event)
{
	return true;	
}


VecD Pendulum::step(VecD x0, double dt, const NEAT::Network& net, ODEsolver& eq){
	while(x0[1]>2.*M_PI)  x0[1] -= 4.*M_PI;
	while(x0[1]<-2.*M_PI) x0[1] += 4.*M_PI;

	double AA = 0.;
	VecD inputs(4);
	inputs[0] = (x0[1]/thetaMAX 	+ 1.)/2.;
	inputs[1] = (x0[2]/xMAX 		+ 1.)/2.;
	inputs[2] = (x0[3]/thetaDMAX 	+ 1.)/2.;
	inputs[3] = (x0[4]/xDMAX 		+ 1.)/2.;
	AA = A_MAGN * (2*(int)(net.evaluate(inputs)[0]+0.5)-1);
	eq.setParams({ AA });
	return eq.stepRK4(x0, dt);
}

bool Pendulum::update(const float fElapsedTime, const float fTotalTime)
{
	if(!isdead)
	{
		x0 = step(x0, fElapsedTime * SIM_SPEED, net, eq);

		move(x0[2],x0[1]);

		bool out = (x0[0]<TMAX_SFML && fabs(x0[1])<=thetaMAX && fabs(x0[2])<=xMAX);

		isdead = !out;
	}
	return !isdead;
}

bool Pendulum::draw()
{
	m_target->draw(base);
	m_target->draw(line);
	m_target->draw(circle);
	return true;
}

void Pendulum::move(float pos, float angle) {
	pos = toPixels(pos+xMAX)-base.getGlobalBounds().width/4.;

	base.setPosition(pos, base.getPosition().y);
	line.line[0].position.x = pos + base.getGlobalBounds().width / 4;
	line.line[1].position.x = line.line[0].position.x - m_lenght * sin(angle);
	line.line[1].position.y = line.line[0].position.y - m_lenght * cos(angle);

	circle.setPosition(line.line[1].position);
}
