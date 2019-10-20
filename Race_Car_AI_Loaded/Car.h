#pragma once

#include "Game_Framework/Global.h"
#include "Game_Framework/holders.h"
#include "Track.h"

#define LATERAL_FRICTION 4.				// seg^-1
#define BACKWARDS_FRICTION 0.5			// seg^-1
#define SPEED_ACC (0.28*SCREENWIDTH)	// pix/seg^2
#define STEER_FACTOR (1500./SCREENWIDTH)	// pix^-1

#define CAR_SENSORS_DIST (int)SCREENWIDTH //pixels

class Car : public GF::Widget, public sf::Sprite {
public:

	Car();

	Car(sf::RenderTarget*, float x, float y, float size_x, float size_y, Track* track, std::string image = "");
	Car(sf::RenderTarget*, sf::Vector2f pos, float size_x, float size_y, Track* track, std::string image = "");
	Car(sf::RenderTarget*, float x, float y, float size_x, float size_y, Track* track, const sf::Texture& texture);
	Car(sf::RenderTarget*, sf::Vector2f pos, float size_x, float size_y, Track* track, const sf::Texture& texture);

	bool handleEvent(GF::Event& event) override;
	bool update(const float fElapsedTime, const float fTotalTime) override;
	bool draw() override;

	bool isClicked();
	void steer_gas(float x, float y);

	inline int getCheckpoint() const { return current_checkpoint; }

	std::vector<double> getSensors();
	void reset();

	double t;
	bool alive;

private:
	
	void setup(float x, float y, float size_x, float size_y, std::string image = "");
	void setup(float x, float y, float size_x, float size_y, const sf::Texture& texture);

	void boost();
	void drive(const float fElapsedTime);

	void setImage(float size_x, float size_y, std::string image_path);
	void setImage(float size_x, float size_y, const sf::Texture& texture);

	void updateSensors();

	void showSensors(bool b);

private:

	float vx = 0., vy = 0., ax = 0., ay = 0.;

	float steer_rate; //auxiliary save from 'handleEvent' to 'update'

	bool show_sensors;

	sf::Texture texture;

	Track* track;
	int current_checkpoint;

	std::vector<sf::Vector2f> sensors;
};