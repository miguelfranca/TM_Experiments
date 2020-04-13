#include "Global2.h"
#include "Game_Framework/Global.h"

float random(float begin, float end) {return ((float)rand()) / RAND_MAX * (end - begin) + begin;}

Bot::Bot()
{
	restart();
};

void Bot::restart()
{
	setSpeed(speed_default);
	srand((unsigned)time(NULL));
	shape = Bot::shape_default;
	setDamage(damage_default);
	setHealth(health_default);
	setActive(false); //constructor already does this, so this is for real restarts only
}

void Bot::setUp()
{
	static float perimeter_y = SCREENHEIGHT - boundary_y;
	static float perimeter = perimeter_y * 2 + SCREENWIDTH;
	float random_loc = random(0, perimeter);

	if (random_loc <= perimeter_y) {
		Figure::setUp(0, SCREENHEIGHT - random_loc);
	}
	else if (random_loc < perimeter - perimeter_y) {
		Figure::setUp(random_loc - perimeter_y, boundary_y);
	}
	else {
		Figure::setUp(SCREENWIDTH, random_loc - SCREENWIDTH - perimeter_y + boundary_y);
	}

	setVel(0., 0.);
	reactionTime = 0.;

	setActive(true);
	setHealth(health_default);
}

void Bot::move(float rect_x, float rect_y, float fElapsedTime)
{
	if (!getActive()) return;

	reactionTime += fElapsedTime;

	if (reactionTime >= dumbness) {
		float norm = sqrt((rect_x - getX()) * (rect_x - getX()) + (rect_y - getY()) * (rect_y - getY()));

		if (norm == 0)	setVel(0., 0.);
		else			setVel((rect_x - getX()) / norm, (rect_y - getY()) / norm);

		reactionTime = 0.;
	}

	Figure::move(fElapsedTime);

}