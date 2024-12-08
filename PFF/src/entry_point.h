
#include "project/project_data_struct.h"
#include "application.h"

extern PFF::application* PFF::create_application();


#ifdef PFF_PLATFORM_WINDOWS

#include <windows.h>
#include <iostream>

int wmain(int argc, wchar_t* argv[]) {

    std::wcout << L"Number of arguments: " << argc << std::endl;
    for (int i = 0; i < argc; i++) {
        std::wcout << L"Argument " << i << L": " << argv[i] << std::endl;
    }

    std::vector<std::string> args;                  // Convert wide arguments to standard strings
    for (int i = 0; i < argc; i++) {
        int size = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr);
        std::string arg(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, &arg[0], size, nullptr, nullptr);
        arg.pop_back(); // Remove null terminator
        args.push_back(arg);
    }

    auto app = PFF::create_application();
    app->run();
    delete app;

    return EXIT_SUCCESS;
}

#elif defined(PFF_PLATFORM_LINUX)

int main(int argc, char* argv[]) {

    std::vector<std::string> args;                  // Convert char* arguments to strings
    for (int i = 0; i < argc; i++) {
        args.emplace_back(argv[i]);
    }

    auto app = PFF::create_application();
    app->run();
    delete app;

    return EXIT_SUCCESS;
}

#endif
