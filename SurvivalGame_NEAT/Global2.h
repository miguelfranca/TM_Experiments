#pragma once
#include "GraphicsFramework/Global.h"
#include "Player.h"
#include "Shot.h"
#include "Bot.h"
#include "Button.h"

#include <cmath>

#define PI 3.14159265358979f
#define delay 30.f

const std::string MEDIA_PATH = "res/media";

extern const float boundary_y;
extern const float boundary_x;
const int max_bots = 50;

const float difficulty_double = 200.f; //number of kills necessary for difficulty to double
const float difficulty_armor = 3.f; //number of kills necessary for difficulty to double the armor

const float price_increase = 1.5f;
