#pragma once
#include <vector>
#include "Game_Framework/holders.h"
#include "Game_Framework/sfml.h"
#include "Game_Framework/Global.h"
#include <vector>

#define TRACK_RADIUS 80

class Track : public GF::Widget{
public:
	Track(sf::RenderTarget* renderer);

	~Track();
	
	bool draw() override;

	bool handleEvent(GF::Event &event) override;

	void addCircle(sf::Vector2f pos);

	inline sf::Image getTrack() { return image; }

	bool isAtCheckpoint(int obj_checkpoint, sf::Vector2f pos);

	sf::Sprite flipImage();

private:
	std::vector<GF::Circle*> circles;
	std::vector<int> checkpoints;
	sf::RenderTexture track;
	sf::Image image;

};