
#include <util/pffpch.h>

#ifdef PFF_PLATFORM_WINDOWS
    #include <Windows.h>
    #include <commdlg.h>
    #include <iostream>
    #include <tchar.h> // For _T() macros

#elif defined PFF_PLATFORM_LINUX 
    #include <sys/time.h>
    #include <ctime>
    #include <unistd.h>             // for [get_executable_path]
    #error TODO: import libs needed for [file_dialog]

#elif defined PFF_PLATFORM_MAC
    #include <mach-o/dyld.h>        // for [get_executable_path]
    #error TODO: import libs needed for [file_dialog]

#endif

#include "util.h"

namespace PFF::util {


}
