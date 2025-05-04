
#include "application.h"
#include "util/crash_handler.h"

extern PFF::application* PFF::create_application();


#if defined(PFF_PLATFORM_WINDOWS)

#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdLine, int cmdshow) {

#if PFF_DEBUG                                       // open Konsole when in debug
    PFF::util::open_console("PFF Engine - Debug Console", true);
#endif

    PFF::attach_crash_handler();                    // catch any crashes from here on (dont need to call "PFF::detach_crash_handler();" because crash hander is needed for the entire application)
    auto app = PFF::create_application();
    app->init_engine(__argc, __argv);
    app->run();
    delete app;

    return EXIT_SUCCESS;
}

#elif defined(PFF_PLATFORM_LINUX)                   // dont need to create console for linux, because linux!

int main(int argc, char* argv[]) {

    PFF::attach_crash_handler();                    // catch any crashes from here on (dont need to call "PFF::detach_crash_handler();" because crash hander is needed for the entire application)
    auto app = PFF::create_application();
    app->init_engine(argc, argv);
    app->run();
    delete app;
    
    return EXIT_SUCCESS;
}

#endif
