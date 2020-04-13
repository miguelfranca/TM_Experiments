#pragma once

#include "Game_Framework/Global.h"
#include "Game_Framework/sfml.h"

#include "Game_Framework/GUI.h"
#include "Game_Framework/Tools.h"
#include "Game_Framework/main.h"
#include "Pendulum.h"
#include "UserDoublePendulum.h"
#include "UserPendulum.h"

#include <iostream>

class MyGame : public GF::Game
{
  public:
    MyGame();

    void updatePopulation(const Population<NEAT::Network> *pop, unsigned n = 1);

    // called once before the game starts
    bool onCreate() override;

    // first thing to be called every frame
    bool onHandleEvent(GF::Event &event) override;

    // called every frame before draw
    bool onUpdate(const float fElapsedTime, const float fTotalTime) override;

    // last thing to be called every frame
    bool onDraw() override;

    void onDestroy();

  private:
    std::vector<Pendulum *> pendulums;
    const Population<NEAT::Network> *pop;
    NEAT::Image *image;
    sf::Sprite network_im;
    unsigned N;

    enum STATES
    {
        TRAIN_AI,
        BEST_AI,
        DOUBLE,
        SINGLE
    };
    int game_state = DOUBLE;

    GF::Text AI_Active;
    GF::Text time;
    GF::Text generation;
    GF::Text population;
    GF::Text fitness;
    unsigned generation_c = 0;

    UserDoublePendulum *player_double;
    UserPendulum *player_single;
    Pendulum *BestAI;
};
