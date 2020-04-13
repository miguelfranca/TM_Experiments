#pragma once

#include <SFML/Graphics.hpp>

#include "MyShape.h"

class Figure
{
public:
	Figure(float xx = 0, float yy = 0, float vxx = 0, float vyy = 0, float sspeed = 0);

	virtual void move(float vxx, float vyy, float fElapsedTime) { setVel(vxx, vyy); move(fElapsedTime); }
	virtual void move(float fElapsedTime);

	void setUp(float xx, float yy);
	void setVel(float vxx, float vyy);

	void setSpeed(float sspeed) { speed = sspeed; }

	float getX() const { return x; }
	float getY() const { return y; }
	float getVX() const { return vx; }
	float getVY() const { return vy; }

	bool collide(Figure& fig) { return collide(*this, fig); }
	static bool collide(Figure& fig1, Figure& fig2);

	virtual void draw(sf::RenderWindow& window) { if (active) shape.draw(window); }

	virtual sf::Vector2f getSize() = 0;

	static float getMaxHealth() { return max_health; }

	void setHealth(float h, bool color = true) { health = h; if (color && h >= 0) setColor(); }
	float getHealth() const { return health; }

	void setDamage(float h) { damage = h; }
	float getDamage() const { return damage; }

	void setActive(bool b) { active = b; }
	bool getActive() const { return active; }

protected:
	MyShape shape;
	void setColor();
	bool active = false;

private:
	float damage, health;

	static const float max_health;

	float x, y;
	float vx, vy;
	float speed;

	static float reduce;
	float speedY(float fElapsedTime);
};

float rainbow(float x);
sf::Color color(float x);
sf::Color color_2(float x, float compress = 0.1f);