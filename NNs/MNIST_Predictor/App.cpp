#include "App.h"
#include <SFML/Window.hpp>

App::App(const std::string title) : net("MNIST_network.txt"), radius(DEFAULT_RADIUS)
{
	this->title = title;
	setupWindow(700, 500, CENTER_SCREEN - sf::Vector2i(700 / 2, 500 / 2), sf::Style::Default);
	setStaticScreen(true);
}

// called once before the game starts
bool App::onCreate()
{
	return true;
}

int strangle(int x, int min, int max)
{
	return x < min ? min : x > max ? max : x;
}

// first thing to be called every frame
bool App::onHandleEvent(GF::Event& event)
{
	static GF::ToggleKey C(sf::Keyboard::C);

	if (GF::Mouse::Left.isPressed())
		window.draw(GF::Circle(radius, GF::Mouse::getPosition(window)));

	if (GF::Mouse::Right.isPressed())
		window.draw(GF::Circle(radius, GF::Mouse::getPosition(window), BLACK));

	if (C.isKeyPressedOnce(event))
		window.clear();

	if (GF::Mouse::Wheel.moved(event)) {
		int delta = GF::Mouse::Wheel.delta(event);
		radius += delta;
		radius = strangle(radius, MIN_RADIUS, MAX_RADIUS);
	}

	return true;
};

// called every frame before draw
bool App::onUpdate(const float fElapsedTime, const float fTotalTime)
{
	return true;
};

// last thing to be called every frame
bool App::onDraw()
{
	return true;
};

void App::App::onDestroy()
{

}

void App::onSwitch(std::string lastState)
{

}
