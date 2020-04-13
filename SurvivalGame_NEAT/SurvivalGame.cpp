#include "SurvivalGame.h"
#include "SBestReplay.h"

const int SurvivalGame::buttons_number = 4;
Button SurvivalGame::button[buttons_number];
std::string SurvivalGame::texts[buttons_number] = { "Damage", "BPS", "Regenerate", "Armor" };
int (*SurvivalGame::func[buttons_number])(Player&, int) = { Player::increaseDamage, Player::increaseBPS, Player::Regenerate, Player::increaseArmor };
float SurvivalGame::button_Width = SW * 94;
MyShape SurvivalGame::cursor;
MyShape SurvivalGame::ground;
MyShape SurvivalGame::dollarSign;
sf::Text SurvivalGame::text;
sf::Font SurvivalGame::font;
sf::SoundBuffer SurvivalGame::buffer;
sf::Sound SurvivalGame::bullet_sound;
sf::Sound SurvivalGame::numbers;
sf::Sound SurvivalGame::start;

SurvivalGame::SurvivalGame(std::string t) : bestState(nullptr)
{
	title = t;
	game.player.setUp(SW * 300., SH * 300.);
}

// called once before the loop starts
bool SurvivalGame::onCreate()
{
	window.setMouseCursorVisible(false);
	window.setPosition(sf::Vector2i(0, 0));

	cursor.setFile(MEDIA_PATH + "/cursor.png", SW * 0.08f, SH * 0.08f);
	cursor.setOrigin(0, 0);
	ground.setFile(MEDIA_PATH + "/ground_cut.png", SCREENWIDTH / 1600., SCREENHEIGHT / 1000.);
	ground.setPosition(SCREENWIDTH / 2.f, SCREENHEIGHT / 2.f);
	dollarSign.setFile(MEDIA_PATH + "/dollarSign.png", SW * 0.09f, SH * 0.09f);
	dollarSign.setPosition(SW * 15, SH * 57);

	font.loadFromFile(MEDIA_PATH + "/fonts/5.ttf");
	text.setFont(font);

	buffer.loadFromFile(MEDIA_PATH + "/sounds/bulletsound3.wav");
	bullet_sound.setBuffer(buffer);
	bullet_sound.setVolume(5);

	float button_Width = SW * 94;

	for (int i = 0; i < buttons_number; i++) {

		button[i].setUp(button_Width, SH * 40.f, sf::Color::Black,
		                (float)(SW * 350 + (button_Width + SW * 30) * i), SH * 30.f);
		button[i].setText(texts[i], sf::Color::Red, (int)(SH * 15), MEDIA_PATH + "/fonts/5.ttf");
		button[i].setSubText(200, sf::Color::Red, (int)(SH * 10));
		button[i].setFunction(func[i]);
	}

	return true;
}

bool SurvivalGame::step(GameEntities& enti, bool mousePressed, float mousePosX, float mousePosY,
                        float horizontalMovement, float verticalMovement, int buttonUpgrade, float elapsedTime)
{
	enti.botPeriod += elapsedTime;
	enti.shotsPeriod += elapsedTime;
	enti.botDamagePeriod += elapsedTime;

	if (buttonUpgrade >= 0) {
		button[buttonUpgrade].Upgrade(enti.player);
	}

	if (mousePressed && enti.shotsPeriod >= enti.player.getPeriodD()) {
		enti.player.setUpShot(mousePosX, mousePosY,
		                      enti.player.getX(), enti.player.getY());
		enti.shotsPeriod = 0;
	}

	if (!(((enti.player.getX() < (enti.player.getSize().x / 2) + boundary_x) && horizontalMovement < 0.)
	      || ((enti.player.getX() > SCREENWIDTH - enti.player.getSize().x / 2 - boundary_x)
	          && horizontalMovement > 0.)
	      || ((enti.player.getY() < (enti.player.getSize().y / 2) + boundary_y) && verticalMovement < 0.)
	      || ((enti.player.getY() > SCREENHEIGHT - enti.player.getSize().y / 2 - boundary_x)
	          && verticalMovement > 0.))) {

		enti.player.setVel(horizontalMovement, verticalMovement);
		enti.player.move(elapsedTime);
	}

	for (int i = 0; i < max_bots; i++)
		enti.bots[i].move(enti.player.getX(), enti.player.getY(), elapsedTime);

	enti.player.move_shots(elapsedTime);


	if (enti.botPeriod >= Bot::getPeriodD()) {
		enti.bots[enti.bot_number++].setUp();
		enti.botPeriod = 0;

		if (enti.bot_number == max_bots) enti.bot_number = 0;
	}

	if (enti.botDamagePeriod > Bot::getDamageRate()) {
		for (int i = 0; i < max_bots; i++)
			enti.player.collide(enti.bots[i]); // returns true when player DIES

		enti.botDamagePeriod = 0.;
	}

	return (enti.shotsPeriod == 0);
}

// first thing to be called every frame
bool SurvivalGame::onHandleEvent(GF::Event& event)
{
	static GF::ToggleKey SPACE(sf::Keyboard::Space);
	static GF::ToggleKey E(sf::Keyboard::E);

	if (SPACE.isKeyReleasedOnce(event))
		paused = !paused;

	button_pressed = -1;

	if (GF::Mouse::Left.clicked(event)) {
		for (unsigned i = 0; i < buttons_number; ++i) {
			if (button[i].isClicked(GF::Mouse::getPosition(window).x, GF::Mouse::getPosition(window).y))
				button_pressed = i;
		}
	}

	if (E.isKeyReleasedOnce(event)) {
		if (bestState == nullptr) {
			bestState = new SBestReplay(&states, &window);
			bestState->setMaxFPS(getMaxFPS());
		}

		states.change("Evolution", bestState);
	}


	return true;
}

// called every frame before draw
bool SurvivalGame::onUpdate(const float fElapsedTime, const float fTotalTime)
{

	float velX = 0, velY = 0, mouseX = 0, mouseY = 0;
	bool mousePressed = false;

	if (GF::Mouse::Left.isPressed()) {
		mousePressed = true;
		mouseX = GF::Mouse::getPosition(window).x;
		mouseY = GF::Mouse::getPosition(window).y;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		velX = -1.;
		velY = 0.;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)
	    || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		velX = 1.;
		velY = 0.;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		velX = 0.;
		velY = -1.;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		velX = 0.;
		velY = 1.;
	}

	if (paused)
		step(game, false, 0., 0., 0., 0., button_pressed, 0.);
	else if (step(game, mousePressed, mouseX, mouseY, velX, velY, button_pressed, fElapsedTime))
		bullet_sound.play();


	return true;
}

// last thing to be called every frame
bool SurvivalGame::onDraw()
{
	window.draw(ground);

	game.player.draw(window, paused);
	game.player.drawShots(window);

	int digitsS = game.player.getScore() ? 1 + (int)log10(game.player.getScore()) : 0;
	int digitsM = game.player.getMoney() ? 1 + (int)log10(game.player.getMoney()) : 0;
	window.draw(GF::Text(std::to_string(game.player.getScore()),
	                     sf::Vector2f(SW * (110.f + 10.f * digitsS), SH * 20.f),
	                     (int)(SH * 20), BLACK, GF::TOPLEFT));
	window.draw(GF::Text("Score:", sf::Vector2f(SW * 50.f, SH * 20.f), (int)(SH * 20), BLACK,
	                     GF::TOPLEFT));
	window.draw(GF::Text(std::to_string(game.player.getMoney()),
	                     sf::Vector2f(SW * (35.f + 5.f * digitsM), SH * 54.f),
	                     (int)(SH * 20), YELLOW, GF::TOPLEFT));

	for (int i = 0; i < buttons_number; i++) button[i].draw(window);

	for (int i = 0; i < max_bots; i++) {
		game.bots[i].draw(window);
	}

	cursor.setPosition((float)sf::Mouse::getPosition(window).x,
	                   (float)sf::Mouse::getPosition(window).y);
	window.draw(cursor);

	window.draw(dollarSign);

	if (paused)
		window.draw(GF::Text("Pause", CENTER_WINDOW, (int)(SH * 100), RED));

	return true;
}

// called before exiting the app
void SurvivalGame::onDestroy()
{
	if (bestState != nullptr)
		delete bestState;
}
