#pragma once
#include <vector>
#include "Game_Framework/holders.h"
#include "Game_Framework/sfml.h"
#include "Game_Framework/GUI.h"
#include "Game_Framework/Global.h"
#include "Utility.h"
#include <vector>
#include <math.h>

#define TRACK_RADIUS 40 * SW

class Track : public GF::Widget{
public:
	Track(sf::RenderTarget* renderer);

	~Track();
	
	bool draw() override;

	inline sf::Vector2f start_pos() const { return (circles.size() > 0) ? circles[0]->getPosition() : sf::Vector2f(0, 0); } 
	inline double start_rotation() const
	{
		if(checkpoints.size() > 1)
			return toDegrees(atan ((circles[0]->getPosition().y - circles[checkpoints[1]]->getPosition().y) 
								  / (circles[0]->getPosition().x - circles[checkpoints[1]]->getPosition().x))) + 90;
		else
			return 0.;
	}

	bool handleEvent(GF::Event &event) override;

	void addCircle(sf::Vector2f pos, int index = -1);

	inline sf::Image& getTrack() { return image; }

	bool isAtCheckpoint(int obj_checkpoint, sf::Vector2f pos);

	inline int getMaxCheckpoint() const { return checkpoints.size(); }

	void saveAndFlip();

	bool isOverButton();

	void save(const std::string &filename) const ;

private:
	std::vector<GF::Circle*> circles;
	std::vector<int> checkpoints;
	sf::RenderTexture track;
	sf::Image image;
	sf::Sprite* sprite;
	GF::Button<GF::Rectangle> show_button;
	bool show;
};