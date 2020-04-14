#pragma once

#include <iostream>

#include "Game_Framework/GUI.h"
#include "Game_Framework/Tools.h"
#include "Game_Framework/main.h"
#include "Game_Framework/sfml.h"

#include "Image.hpp"
#include "NEAT_GA.hpp"

#include "Global2.h"

#define BEST_FITNESS 120

class SBestReplay;

struct GameEntities
{
    Player player;
    int bot_number = 0;
    Bot bots[max_bots];
    float botPeriod = 0.;
    float shotsPeriod = 0.;
    float botDamagePeriod = 0.;
};

class SurvivalGame : public GF::Game
{
  public:
    SurvivalGame(std::string t = "SurvivalGame");

    // called once before the loop starts
    bool onCreate() override;

    // first thing to be called every frame
    bool onHandleEvent(GF::Event &event) override;

    // called every frame before draw
    virtual bool onUpdate(const float fElapsedTime,
                          const float fTotalTime) override;

    // last thing to be called every frame
    bool onDraw() override;

    // called once before exiting the app
    void onDestroy();

    void onSwitch(std::string other) {}

    // GAME
    static bool step(GameEntities &, bool mousePressed, float mousePosX,
                     float mousePosY, float verticalMovement,
                     float horizontalMovement, int buttonUpgrade,
                     float elapsedTime);

    static const Button *getButtons() { return button; }

  protected:
    // GAME
    GameEntities game;

    static const int buttons_number;
    static Button button[];
    int button_pressed = -1;
    static std::string texts[];
    static int (*func[])(Player &, int);
    static float button_Width;

    bool paused = false;

    static MyShape cursor;
    static MyShape ground;
    static MyShape dollarSign;
    static sf::Text text;
    static sf::Font font;
    static sf::SoundBuffer buffer;
    static sf::Sound bullet_sound;
    static sf::Sound numbers;
    static sf::Sound start;

    SBestReplay *bestState;
};
