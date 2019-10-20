#include "CarAI.h"

int main() {
	// GA::generator.setSeed(1854723812);
	// std::cout << GA::generator.getSeed() << std::endl;

	CarAI gg;
	gg.showFPS(true);
	gg.setMaxFPS(60);
	gg.run();

	//system("pause");
	return 0;
}
