#include "Track.h"

#include <fstream>

Track::Track(sf::RenderTarget* renderer) : GF::Widget(renderer), show_button(renderer,
	        sf::Vector2f(100 * SW, 50 * SH),
	        sf::Color::White, sf::Color::Black, sf::Color::Black, sf::Color::White), show(true)
{
	if (!track.create(SCREENWIDTH, SCREENHEIGHT))
		exit(1);

	track.clear(sf::Color::Transparent);

	image.create(SCREENWIDTH, SCREENHEIGHT);

	checkpoints.push_back(0);

	sprite = new sf::Sprite(track.getTexture());

	show_button.setTextSize(35 * SW);
	show_button.setText(std::string("Show/Hide\nTrack"));
	show_button.setPos(sf::Vector2f(250 * SW, 50 * SH));
}

Track::~Track()
{
	for (auto& circle : circles)
		delete circle;

	circles.clear();

	if (sprite) delete sprite;
}

bool Track::draw()
{
	if (show)
		m_target->draw(*sprite);

	show_button.draw();
	return true;
}

void Track::addCircle(sf::Vector2f pos, int index)
{
	if (index == -1) {
		GF::Circle* circle  = new GF::Circle(TRACK_RADIUS, pos, GRAY);
		circles.push_back(circle);
		track.draw(*circle);
	}

	if (dist(pos, circles[checkpoints.back()]->getPosition()) >= 100. * SW)
		checkpoints.push_back(index == -1 ? (circles.size() - 1) : index);
}

bool Track::handleEvent(GF::Event& event)
{
	if (GF::Mouse::Right.isPressed()) {
		auto end = std::remove_if(circles.begin(), circles.end(),
		[this](GF::Circle * const & circle) {
			return (*circle).isRolledOn((sf::RenderWindow&)(
			                                *m_target)); // removes if mouse position is inside circle boundaries
		});

		circles.erase(end, circles.end());

		track.clear(sf::Color::Transparent);
		checkpoints.clear();
		checkpoints.push_back(0);

		for (unsigned i = 0; i < circles.size(); i++) {
			addCircle(circles[i]->getPosition(), i);
			track.draw(*circles[i]);
		}
	}

	if (GF::Mouse::Left.released(event) || GF::Mouse::Right.released(event))
		image = track.getTexture().copyToImage();

	show_button.handleEvent(event);

	if (show_button.isClicked(event, (sf::RenderWindow&)(*m_target)))
		show = !show;

	return true;
}

bool Track::isAtCheckpoint(int obj_checkpoint, sf::Vector2f pos)
{
	if (circles.size() == 0) return false;

	obj_checkpoint %= checkpoints.size();

	// if(obj_checkpoint >= (int)circles.size()) return false;
	if (obj_checkpoint >= (int)checkpoints.size()) return false;

	if (pos.y < 0 || pos.y > SCREENHEIGHT || pos.x > SCREENWIDTH || pos.x < 0) return false;

	// std::cout << "Checkpoint + 1: " << circles[checkpoints[obj_checkpoint]]->contains(pos) << std::endl;

	return circles[checkpoints[obj_checkpoint]]->contains(pos);
}


void Track::saveAndFlip()
{
	if (sprite) delete sprite;

	sprite = new sf::Sprite(track.getTexture());
	sprite->setScale(1.0, -1.0);
	sprite->setPosition(0.0, sprite->getTextureRect().height);
}

bool Track::isOverButton()
{
	return show_button.isRolledOn((sf::RenderWindow&)(*m_target));
}

void Track::save(const std::string& filename) const
{
	std::ofstream file;
	file.open(filename, std::ios::out);

	for (unsigned i = 0; i < circles.size(); ++i) {
		sf::Vector2f center = circles[i]->getPosition();
		file << (center.x / SW) << "\t";
		file << (center.y / SH) << std::endl;
	}

	file.close();
}

void Track::load(const std::string& filename)
{
	track.clear(sf::Color::Transparent);
	circles.clear();
	checkpoints.clear();

	std::ifstream file;
	file.open(filename, std::ios::in);

	while (file) {
		sf::Vector2f center;
		file >> center.x;
		file >> center.y;
		center.x *= SW;
		center.y *= SH;
		if (center.x == 0 && center.y == 0) continue;
		addCircle(center);
	}

	file.close();

	saveAndFlip();
}
