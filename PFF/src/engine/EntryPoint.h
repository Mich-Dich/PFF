#pragma once

#ifdef PFF_PLATFORM_WINDOWS

extern PFF::application* PFF::Create_application();

void main(int argc, char** argv) {

	auto app = PFF::Create_application();
	app->Run();
	delete app;

}

#endif // PFF_PLATFORM_WINDOWS
