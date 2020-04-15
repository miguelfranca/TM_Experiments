#include "SurvivalGame.h"

int main()
{

	SurvivalGame gg;
	gg.showFPS(false);
	gg.setMaxFPS(120);
	gg.run();

	return 0;
}
