
#include "util/pffpch.h"

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

#include "system.h"


namespace PFF::util {


    bool run_program(const std::filesystem::path& path_to_exe, const std::string& cmd_args, bool open_console) { return run_program(path_to_exe, cmd_args.c_str(), open_console); }


    bool run_program(const std::filesystem::path& path_to_exe, const char* cmd_args, bool open_console) {

        CORE_LOG(Trace, "executing program at [" << path_to_exe.generic_string() << "]");

        bool result = false;

#ifdef PFF_PLATFORM_WINDOWS

        STARTUPINFOA startupInfo;
        PROCESS_INFORMATION processInfo;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        ZeroMemory(&processInfo, sizeof(processInfo));

        std::string cmdArguments = path_to_exe.generic_string() + " " + cmd_args;
        auto working_dir = util::get_executable_path().generic_string();

        // Start the program
        result = CreateProcessA(
            NULL,							            // Application Name
            (LPSTR)cmdArguments.c_str(),	            // Command Line Args
            NULL,							            // Process Attributes
            NULL,							            // Thread Attributes
            FALSE,							            // Inherit Handles
            (open_console) ? CREATE_NEW_CONSOLE : 0,	// Creation Flags
            NULL,							            // Environment
            working_dir.c_str(),			            // Current Directory
            &startupInfo,					            // Startup Info
            &processInfo					            // Process Info
        );

        WaitForSingleObject(processInfo.hProcess, INFINITE);                                        // Wait for the process to finish

        if (result) {                                                                               // Close process and thread handles

            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        } else
            CORE_LOG(Error, "Unsuccessfully started process: " << path_to_exe.generic_string());

#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC
    #error Not implemented yet
#endif

        return result;
    }



    void high_precision_sleep(f32 duration_in_milliseconds) {

        static const f32 estimated_deviation = 10.0f;
        auto loc_duration_in_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<f32>(duration_in_milliseconds)).count();
        auto target_time = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(static_cast<int>(loc_duration_in_milliseconds));

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(loc_duration_in_milliseconds - estimated_deviation)));

        // Busy wait for the remaining time
        while (std::chrono::high_resolution_clock::now() < target_time)
            ;

        //auto actual_sleep_time = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - target_time + std::chrono::milliseconds(static_cast<int>(duration_in_milliseconds)) ).count();
        //CORE_LOG(Debug, "left over time: " << actual_sleep_time << " ms");
    }


    system_time get_system_time() {

        system_time loc_system_time{};

#ifdef PFF_PLATFORM_WINDOWS

        SYSTEMTIME win_time;
        GetLocalTime(&win_time);
        loc_system_time.year = static_cast<u16>(win_time.wYear);
        loc_system_time.month = static_cast<u8>(win_time.wMonth);
        loc_system_time.day = static_cast<u8>(win_time.wDay);
        loc_system_time.day_of_week = static_cast<u8>(win_time.wDayOfWeek);
        loc_system_time.hour = static_cast<u8>(win_time.wHour);
        loc_system_time.minute = static_cast<u8>(win_time.wMinute);
        loc_system_time.secund = static_cast<u8>(win_time.wSecond);
        loc_system_time.millisecends = static_cast<u16>(win_time.wMilliseconds);

#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC

        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm* ptm = localtime(&tv.tv_sec);
        loc_system_time.year = static_cast<u16>(ptm->tm_year + 1900);
        loc_system_time.month = static_cast<u8>(ptm->tm_mon + 1);
        loc_system_time.day = static_cast<u8>(ptm->tm_mday);
        loc_system_time.day_of_week = static_cast<u8>(ptm->tm_wday);
        loc_system_time.hour = static_cast<u8>(ptm->tm_hour);
        loc_system_time.minute = static_cast<u8>(ptm->tm_min);
        loc_system_time.secund = static_cast<u8>(ptm->tm_sec);
        loc_system_time.millisecends = static_cast<u16>(tv.tv_usec / 1000);

#endif
        return loc_system_time;
    }


    std::filesystem::path file_dialog(const std::string& title, const std::vector<std::pair<std::string, std::string>>& filters) {

#ifdef PFF_PLATFORM_WINDOWS
        // Assuming you have a way to get the handle of your main window
        HWND hwndOwner = GetActiveWindow(); // or your main window handle

        OPENFILENAME ofn;                                                       // common dialog box structure
        wchar_t szFile[260] = { 0 };                                            // Using wchar_t instead of char for Unicode support

        ZeroMemory(&ofn, sizeof(ofn));                                          // initialize OPENFILENAME
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwndOwner;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);

        std::wstring filter;
        for (const auto& f : filters)                                           // create filter string
            filter += std::wstring(f.first.begin(), f.first.end()) + L'\0' + std::wstring(f.second.begin(), f.second.end()) + L'\0';
        filter += L'\0';

        ofn.lpstrFilter = filter.c_str();
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        std::wstring wtitle(title.begin(), title.end());                        // set dialog title
        ofn.lpstrTitle = wtitle.c_str();

        if (GetOpenFileNameW(&ofn) == TRUE)
            return std::filesystem::path(szFile);

        return std::filesystem::path();

#elif defined PFF_PLATFORM_LINUX

        GtkWidget* dialog;
        GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
        gint res;

        gtk_init(NULL, NULL);
        dialog = gtk_file_chooser_dialog_new(
            title.c_str(),
            NULL,
            action,
            "_Cancel",
            GTK_RESPONSE_CANCEL,
            "_Open",
            GTK_RESPONSE_ACCEPT,
            NULL
        );

        for (const auto& filter_pair : filters) {                               // Add filters
            GtkFileFilter* filter = gtk_file_filter_new();
            gtk_file_filter_set_name(filter, filter_pair.first.c_str());

            // Split multiple extensions (e.g., "*.cpp;*.h")
            std::stringstream ss(filter_pair.second);
            std::string pattern;
            while (std::getline(ss, pattern, ';')) {
                gtk_file_filter_add_pattern(filter, pattern.c_str());
            }

            gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
        }

        std::filesystem::path result;
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            result = std::filesystem::path(filename);
            g_free(filename);
        }

        gtk_widget_destroy(dialog);
        while (gtk_events_pending()) gtk_main_iteration();

        return result;

#elif defined PFF_PLATFORM_MAC

        @autoreleasepool{
            NSOpenPanel * panel = [NSOpenPanel openPanel];
            [panel setTitle : [NSString stringWithUTF8String : title.c_str()] ] ;
            [panel setCanChooseFiles : YES] ;
            [panel setCanChooseDirectories : NO] ;
            [panel setAllowsMultipleSelection : NO] ;

            // Add filters
            NSMutableArray* fileTypes = [NSMutableArray array];
            for (const auto& filter : filters) {
                std::string extensions = filter.second;
                // Remove the "*." from each extension
                size_t pos = 0;
                while ((pos = extensions.find("*.")) != std::string::npos) {
                    extensions.erase(pos, 2);
                }

                // Split by semicolon and add to allowed types
                std::stringstream ss(extensions);
                std::string ext;
                while (std::getline(ss, ext, ';')) {
                    [fileTypes addObject : [NSString stringWithUTF8String : ext.c_str()] ] ;
                }
            }
            [panel setAllowedFileTypes : fileTypes];

            std::filesystem::path result;
            if ([panel runModal] == NSModalResponseOK) {
                NSURL* url = [panel URL];
                result = std::filesystem::path(std::string([[url path]UTF8String] ));
            }

            return result;
        }

            std::filesystem::path get_executable_path() {
            return std::filesystem::path();
        }

    }

#endif
}


std::filesystem::path get_executable_path() {

#ifdef PFF_PLATFORM_WINDOWS

    wchar_t path[MAX_PATH];
    if (GetModuleFileNameW(NULL, path, MAX_PATH)) {
        std::filesystem::path execPath(path);
        return execPath.parent_path();
    }

#elif defined PFF_PLATFORM_LINUX 

    char path[1024];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0'; // Null-terminate the string
        std::filesystem::path execPath(path);
        return execPath.parent_path();
    }

#elif defined PFF_PLATFORM_MAC

    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        std::filesystem::path execPath(path);
        return execPath.parent_path();
    }

#endif

    std::cerr << "Error retrieving the executable path." << std::endl;
    return std::filesystem::path();
}


}
