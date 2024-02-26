
#include "application.h"

#ifdef PFF_PLATFORM_WINDOWS

extern PFF::application* PFF::create_application();

int main() {

	PFF::Logger::Init("[$B$T:$J$E] [$B$L$X $K - $P:$G$E] $C$Z");

	auto app = PFF::create_application();
	app->run();
	delete app;

	return EXIT_SUCCESS;
}

#endif


/*
ToDo:

	- split maintread into main/physics/render
	- add event to input action (maybe func pointer)
	- create level editor
	- enable switch between dynamic_mesh and static_mesh at runtime
	- add input smoothing
	- add lighting capability
	- add modle import
	- add texture import

*/
