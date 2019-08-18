#include "Track.h"


Track::Track(sf::RenderTarget* renderer) : GF::Widget(renderer)
{ 
	if (!track.create(SCREENWIDTH, SCREENHEIGHT))
		exit(1);
	track.clear(sf::Color::Transparent);

	image.create(SCREENWIDTH, SCREENHEIGHT);

	checkpoints.push_back(0);
}

Track::~Track() 
{
	for (auto& circle : circles)
		delete circle;
	circles.clear();
}

bool Track::draw()
{
	sf::Sprite sprite = flipImage();
	m_target->draw(sprite);
	return true;
}

void Track::addCircle(sf::Vector2f pos)
{
	GF::Circle* circle  = new GF::Circle(TRACK_RADIUS, pos, GRAY);
	circles.push_back(circle);
	track.draw(*circle);

	if(dist(pos, circles[checkpoints.back()]->getPosition()) >= 100.)
		checkpoints.push_back(circles.size() - 1);
}

bool Track::handleEvent(GF::Event &event)
{
	if(GF::Mouse::Right.isPressed()){
		auto end = std::remove_if(circles.begin(), circles.end(),
			[this](GF::Circle* const &circle)
			{	
					return (*circle).isRolledOn((sf::RenderWindow&)(*m_target)); // removes if mouse position is inside circle boundaries
				});

		circles.erase(end, circles.end());

		track.clear(sf::Color::Transparent);
		for(auto& circle : circles)
			track.draw(*circle);
	}

	if(GF::Mouse::Left.released(event) || GF::Mouse::Right.released(event))
		image = track.getTexture().copyToImage();

	return true;
}

bool Track::isAtCheckpoint(int obj_checkpoint, sf::Vector2f pos) 
	{
		if(circles.size() == 0) return false;
		obj_checkpoint %= checkpoints.size();
		// if(obj_checkpoint >= (int)circles.size()) return false;
		if(obj_checkpoint >= (int)checkpoints.size()) return false;

		if(pos.y < 0 || pos.y > SCREENHEIGHT || pos.x > SCREENWIDTH || pos.x < 0 ) return false;

		return circles[checkpoints[obj_checkpoint]]->contains(pos); 
	}


sf::Sprite Track::flipImage()
{
	sf::Sprite sprite(track.getTexture());
	sprite.setScale(1.0, -1.0);
	sprite.setPosition(0.0, sprite.getTextureRect().height);
	return sprite;
}
