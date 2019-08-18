#include "MyGame.h"

int main() {

	MyGame gg;
	gg.showFPS(true);
	gg.setMaxFPS(240);
	gg.run();

	//system("pause");
	return 0;
}
