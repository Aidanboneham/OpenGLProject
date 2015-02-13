#include "AdvancedTextures.h"

int main()
{
	AdvancedTextures app;

	if (app.startup() == false)
	{
		return -1;
	}

	while ( app.update() == true )
	{
		app.draw();
	}

	app.shutdown();

	return 0;
}