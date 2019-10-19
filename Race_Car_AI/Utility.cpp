#include <math.h>
#include "Utility.h"

int randomI(const int begin, const int end) {
	return rand() % (end - begin + 1) + begin;
}

float randomF(const float begin, const float end) {
	return (float)rand() / RAND_MAX * (end - begin) + begin;
}

sf::Color randomColor() {
	return sf::Color(rand() % 255, rand() % 255, rand() % 255);
}

float dTanH(float value) {
	return 1 - (value * value);
}

float toRadians(float angle) {
	return angle * M_PI / 180.f;
}

float toDegrees(float angle) {
	return angle * 180.f / M_PI;
}

sf::Vector2f vector_rotator_matrix(float a1, float a2, float angle) {
	float temp_a1 = a1;
	a1 = a1 * cos(toRadians(angle)) - a2 * sin(toRadians(angle));
	a2 = temp_a1 * sin(toRadians(angle)) + a2 * cos(toRadians(angle));
	return sf::Vector2f(a1, a2);
}