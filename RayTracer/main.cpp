#include "Application.h"

int main()
{
    Application app;
	app.showFPS(false);
	app.setMaxFPS(120);
	app.run();

    return 0;
}
