#include "App.h"

int main()
{
	App app;
	app.showFPS(false);
	app.setMaxFPS(120);
	app.run();
	return 0;
}