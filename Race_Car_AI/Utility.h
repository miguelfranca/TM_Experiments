#pragma once
#include <cstdlib> //NULL, rand()
#include "GraphicsFramework/Global.h"

int randomI(const int begin, const int end);

float randomF(const float begin, const float end);

sf::Color randomColor();

float dTanH(float value);

float toRadians(float angle);

float toDegrees(float angle);

sf::Vector2f vector_rotator_matrix(float a1, float a2, float angle);
