
#include "application.h"

#ifdef PFF_PLATFORM_WINDOWS

extern PFF::application* PFF::create_application();

int main() {

	auto app = PFF::create_application();
	app->run();
	delete app;

	return EXIT_SUCCESS;
}

#endif


/*
ToDo:

	- add event to input action (maybe func pointer)
	- create level editor
	- add input smoothing
	- add lighting capability
	- add modle import
	- add texture import
	- fix delta-time

*/
