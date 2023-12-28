
#include <iostream>

#include "util/util.h"
#include "application.h"

#ifdef PFF_PLATFORM_WINDOWS


extern PFF::application* PFF::create_application();

int main() {

	PFF::Logger::Init("engine.log", "client.log", "[$B$T:$J$E] [$B$L$X $K - $F:$G$E] $C$Z");
	auto app = PFF::create_application();
	app->run();
	delete app;

}

#endif
