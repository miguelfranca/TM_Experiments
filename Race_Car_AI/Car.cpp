#include <math.h>
#include <limits.h>
#include <iostream>

#include "Car.h"
#include "Utility.h"
#include "Game_Framework/sfml.h"

//std::vector<sf::Vertex> newLine(sf::Vector2f pos1, sf::Vector2f pos2);
sf::Vector2f checkForColor(sf::Image& image, sf::Vector2f vec, sf::Vector2f position);
bool isInBoundaries(sf::Vector2f pos);


Car::Car() {
	// speed = 0;
	// turn = 0;
	show_sensors = true;
}

Car::Car(sf::RenderTarget* renderer, float x, float y, float size_x, float size_y, Track* track, std::string image) {
	this->track = track;
	m_target = renderer;
	setup(x, y, size_x, size_y, image);
	// speed = 0;
	// turn = 0;
	show_sensors = true;
}

Car::Car(sf::RenderTarget* renderer, sf::Vector2f pos, float size_x, float size_y, Track* track, std::string image) {
	this->track = track;
	m_target = renderer;
	setup(pos.x, pos.y, size_x, size_y, image);
	// speed = 0;
	// turn = 0;
	show_sensors = true;

	current_checkpoint = -1;
}

Car::Car(sf::RenderTarget* renderer, float x, float y, float size_x, float size_y, Track* track, const sf::Texture& texture) {
	this->track = track;
	m_target = renderer;
	setup(x, y, size_x, size_y, texture);
	// speed = 0;
	// turn = 0;
	show_sensors = true;
}

Car::Car(sf::RenderTarget* renderer, sf::Vector2f pos, float size_x, float size_y, Track* track, const sf::Texture& texture) {
	this->track = track;
	m_target = renderer;
	setup(pos.x, pos.y, size_x, size_y, texture);
	// speed = 0;
	// turn = 0;
	show_sensors = true;
}

void Car::setup(float x, float y, float size_x, float size_y, std::string image) {
	setPosition(x, y);
	if (image != "")
		setImage(size_x, size_y, image);
	setOrigin(size_x / 2.f / getScale().x, size_y / 2.f / getScale().y);
}

void Car::setup(float x, float y, float size_x, float size_y, const sf::Texture & texture) {
	setPosition(x, y);
	setImage(size_x, size_y, texture);
	setOrigin(size_x / 2.f / getScale().x, size_y / 2.f / getScale().y);
}

bool Car::handleEvent(GF::Event& event)
{
	static GF::ToggleKey UP (sf::Keyboard::Up);
	static GF::ToggleKey DOWN (sf::Keyboard::Down);
	static GF::ToggleKey LEFT (sf::Keyboard::Left);
	static GF::ToggleKey RIGHT (sf::Keyboard::Right);
	static GF::ToggleKey SPACE (sf::Keyboard::Space);

	float I_f = 0, I_s = 0;

	if(sf::Joystick::isConnected(0) > 0){
		if (abs(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X)) > 2.f)
			I_s += sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) / 100.f;

		if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::R) + 100 > 0.f)
			I_f += ((sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::R) + 100) / 200.f);

		if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z) + 100 > 0.f)
			I_f += -(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z) + 100) / 200.f;
	}

	if(UP.isKeyPressed())
		I_f += 1.;
	if(DOWN.isKeyPressed())
		I_f += -1.;
	if(LEFT.isKeyPressed())
		I_s += -1.;
	if(RIGHT.isKeyPressed())
		I_s += 1.;

	steer_gas(I_f, I_s);

	if(SPACE.isKeyReleasedOnce(event))
		boost();

	return true;
}

void Car::steer_gas(float I_f, float I_s){
	double v = sqrt(vx*vx+vy*vy);

	double angle = toRadians(getRotation()-90);
	double u_x = cos(angle);
	double u_y = sin(angle);

	double dot_v_with_u_right = vx*u_y - vy*u_x;

	//lateral friction
	ax = - u_y * dot_v_with_u_right * LATERAL_FRICTION;
	ay = + u_x * dot_v_with_u_right * LATERAL_FRICTION;

	//backwards friction
	ax += - vx * BACKWARDS_FRICTION;
	ay += - vy * BACKWARDS_FRICTION;

	ax += u_x * I_f * SPEED_ACC;
	ay += u_y * I_f * SPEED_ACC;

	steer_rate = I_s * v * STEER_FACTOR;
}

bool Car::update(const float fElapsedTime, const float fTotalTime)
{
	drive(fElapsedTime);
	updateSensors();
	return true;
}

bool Car::draw()
{
	if (show_sensors) {
		/////////// DRAWS LINES INITIATING IN THE CAR ////////////////////////
		For2(i, sensors.size()) {
			GF::Line line (getPosition(), sensors[i], sf::Color::Black, sf::Color::Black);
			m_target->draw(line);
		}
		// DRAWS SENSOR CIRCLES
		For2(i, sensors.size()) {
			GF::Circle circle(10, sensors[i], sf::Color(41 * i, -41 * i, 41 * i));
			m_target->draw(circle);
		}
	}
	// DRAWS THE SPRITE OF THE CAR

	m_target->draw(*this);

	if(track->isAtCheckpoint(current_checkpoint + 1, getPosition()))
		current_checkpoint++;

	std::cout << current_checkpoint << std::endl;

	return true;
}

void Car::showSensors(bool b) {
	show_sensors = b;
}

bool Car::isClicked() {
	return this->getGlobalBounds().contains(GF::Mouse::getPosition((sf::RenderWindow&)*m_target));
};

void Car::boost() {
	// speed -= MAX_SPEED * 2;
	vy += vy;
	vx += vx;
}

void Car::stop() {
	vx = 0;
	vy = 0;
	// speed = 0;
	ax = 0;
	ay = 0;
	steer_rate = 0;
}

void Car::drive(const float fElapsedTime) {

	if (getPosition().x > SCREENWIDTH)
		setPosition(0, getPosition().y);
	if (getPosition().x < 0)
		setPosition(SCREENWIDTH, getPosition().y);
	if (getPosition().y > SCREENHEIGHT)
		setPosition(getPosition().x, 0);
	if (getPosition().y < 0)
		setPosition(getPosition().x, SCREENHEIGHT);

	double angle = steer_rate * fElapsedTime;
	rotate(angle);

	vx += ax * fElapsedTime;
	vy += ay * fElapsedTime;
	this->move(vx * fElapsedTime, vy * fElapsedTime);
}

void Car::updateSensors() {
	sf::Image image = track->getTrack();
	image.flipVertically();
	sensors.clear();
	//y = vec.y / vec.x * x	checks points to see if its color is !gray, if this happens then it is a boundary
	sensors.push_back(checkForColor(image, vector_rotator_matrix(-CAR_SENSORS_DIST, 0, getRotation()), getPosition()));
	sensors.push_back(checkForColor(image, vector_rotator_matrix(CAR_SENSORS_DIST, 0, getRotation()), getPosition()));
	sensors.push_back(checkForColor(image, vector_rotator_matrix(0, -CAR_SENSORS_DIST, getRotation()), getPosition()));
	sensors.push_back(checkForColor(image, vector_rotator_matrix(0, CAR_SENSORS_DIST, getRotation()), getPosition()));
	sensors.push_back(checkForColor(image, vector_rotator_matrix(-CAR_SENSORS_DIST, -CAR_SENSORS_DIST, getRotation()), getPosition()));
	sensors.push_back(checkForColor(image, vector_rotator_matrix(CAR_SENSORS_DIST, -CAR_SENSORS_DIST, getRotation()), getPosition()));
}

// std::vector<double> Car::getSensors() {
// 	std::vector<double> s;
// 	For2(i, sensors.size()) {
// 		s.push_back(dist(getPosition(), sensors[i]));
// 	}
// 	return s;
// }

sf::Vector2f checkForColor(sf::Image & image, sf::Vector2f vec, sf::Vector2f position) {
	float a = 2;
	sf::Color color = image.getPixel(position.x, position.y);
	if (vec.x != 0) {
		a = vec.y / vec.x;
	}
	if (abs(a) >= 1) {
		a = vec.x / vec.y;
		for(int i=0; i<CAR_SENSORS_DIST; i+=(abs(vec.y) / vec.y)) {
			if (isInBoundaries(sf::Vector2f(a * i + position.x, i + position.y))) {
				if (image.getPixel(a * i + position.x, i + position.y) != color) {
					return sf::Vector2f(a * i + position.x, i + position.y);
				}
			}
			else
				return sf::Vector2f(a * (i + (abs(vec.y) / vec.y)) + position.x, (i + (abs(vec.y) / vec.y)) + position.y);
		}
	}
	else {
		for(int i=0; i>-CAR_SENSORS_DIST; i+=(abs(vec.x) / vec.x)) {
			if (isInBoundaries(sf::Vector2f(i + position.x, a * i + position.y))) {
				if (image.getPixel(i + position.x, a * i + position.y) != color) {
					return sf::Vector2f(i + position.x, a * i + position.y);
				}
			}
			else
				return sf::Vector2f((i + (abs(vec.x) / vec.x)) + position.x, a * (i + (abs(vec.x) / vec.x)) + position.y);
		}
	}
	return sf::Vector2f(INT_MAX, INT_MAX);
}

void Car::setImage(float size_x, float size_y, std::string image_path) {
	if (!texture.loadFromFile(image_path)) {
		texture.loadFromFile("unavailable.png");
		std::cout << image_path << " not found... switching to unavailable.png" << std::endl;
	}
	setTexture(texture);
	setScale(size_x / getTextureRect().width, size_y / getTextureRect().height);
}


void Car::setImage(float size_x, float size_y, const sf::Texture & t) {
	texture = t;
	setTexture(texture);
	setScale(size_x / getTextureRect().width, size_y / getTextureRect().height);
}

bool isInBoundaries(sf::Vector2f pos) {
	return pos.x < SCREENWIDTH && pos.x > 0 && pos.y < SCREENHEIGHT && pos.y > 0;
}