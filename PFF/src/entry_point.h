
#include "application.h"

#ifdef PFF_PLATFORM_WINDOWS

extern PFF::application* PFF::create_application();

int main() {

	PFF::Logger::Init("[$B$T:$J$E] [$B$L$X $K - $F:$G$E] $C$Z");
	auto app = PFF::create_application();
	
	try {

		app->run();
	} catch (const std::exception& e) {
		LOG(Error, "" << e.what());
	}
	
	delete app;

	return EXIT_SUCCESS;
}

#endif


/*

ToDo:
	- port input system from C-version
	- add ImGui
	- create level editor












*/