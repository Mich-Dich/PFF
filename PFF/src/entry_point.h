
#include "application.h"
#include "util/crash_handler.h"

extern PFF::application* PFF::create_application();


#if defined(PFF_PLATFORM_WINDOWS)

#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdLine, int cmdshow) {

#if PFF_DEBUG                                               // open Konsole

    AllocConsole();
    FILE* p_file;
    freopen_s(&p_file, "CONOUT$", "w", stdout);
    freopen_s(&p_file, "CONOUT$", "w", stderr);
    freopen_s(&p_file, "CONIN$", "r", stdin);
    
    std::cout.clear();                                      // Clear the error state for each of the C++ standard stream objects
    std::cerr.clear();
    std::cin.clear();

    SetConsoleTitle(TEXT("PFF Engine - Debug Console"));

    // Enable ANSI escape codes for the console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        std::cerr << "Error: Could not get handle to console output." << std::endl;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        std::cerr << "Error: Could not get console mode." << std::endl;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
        std::cerr << "Error: Could not set console mode to enable ANSI escape codes." << std::endl;
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
