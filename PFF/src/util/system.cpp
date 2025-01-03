
#include "util/pffpch.h"

#if defined(PFF_PLATFORM_WINDOWS)
    #include <Windows.h>
    #include <commdlg.h>
    #include <iostream>
    #include <tchar.h>              // For _T() macros
#elif defined(PFF_PLATFORM_LINUX)
    #include <sys/types.h>          // For pid_t
    #include <sys/wait.h>           // For waitpid
    #include <unistd.h>             // For fork, execv, etc.
    #include <gtk/gtk.h>            // file dialog
    #include <sys/time.h>
    #include <ctime>
    #include <limits.h>
#else
    #error "OS not supported"
#endif

#include "system.h"


namespace PFF::util {

    //
    bool run_program(const std::filesystem::path& path_to_exe, const std::string& cmd_args, bool open_console) { return run_program(path_to_exe, cmd_args.c_str(), open_console); }

    //
    bool run_program(const std::filesystem::path& path_to_exe, const char* cmd_args, bool open_console) {

        //CORE_LOG(Trace, "executing program at [" << path_to_exe.generic_string() << "]");

        bool result = false;

#if defined(PFF_PLATFORM_WINDOWS)

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

#elif defined(PFF_PLATFORM_LINUX)

    std::string cmdArguments = path_to_exe.generic_string() + " " + cmd_args;                       // Prepare the command line arguments
    
    // Split the command line arguments into a vector
    std::vector<std::string> args;
    std::istringstream iss(cmdArguments);
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }

    // Prepare the argument list for exec
    std::vector<char*> execArgs;
    for (auto& a : args) {
        execArgs.push_back(&a[0]);
    }
    execArgs.push_back(nullptr);                                                                    // execv expects a null-terminated array

    pid_t pid = fork();
    if (pid == -1) {
        // Fork failed
        std::cerr << "Failed to fork process." << std::endl;
        return false;
    } else if (pid == 0) {
        // Child process
        if (open_console) {
            // If you want to open a new console, you can use a terminal emulator like xterm
            execlp("xterm", "xterm", "-e", execArgs[0], (char*)nullptr);
        } else {
            // Execute the program
            execv(path_to_exe.c_str(), execArgs.data());
        }
        // If execv fails
        std::cerr << "Failed to execute program: " << path_to_exe.generic_string() << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to finish
        result = WIFEXITED(status) && (WEXITSTATUS(status) == 0);
    }

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

#if defined(PFF_PLATFORM_WINDOWS)

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

#elif defined(PFF_PLATFORM_LINUX)

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

    #if defined(PFF_PLATFORM_WINDOWS)
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

    #elif defined(PFF_PLATFORM_LINUX)

        GtkWidget *dialog;
        std::filesystem::path selected_path;

        gtk_init(nullptr, nullptr);                     // Initialize GTK

        // Create a file chooser dialog
        dialog = gtk_file_chooser_dialog_new(title.c_str(), nullptr, GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, nullptr);
        
        // Add filters to the dialog
        for (const auto& filter : filters) {
            GtkFileFilter *file_filter = gtk_file_filter_new();
            gtk_file_filter_set_name(file_filter, filter.first.c_str());
            gtk_file_filter_add_pattern(file_filter, filter.second.c_str());
            gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter);
        }

        // Show the dialog and wait for user response
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            selected_path = std::filesystem::path(filename);
            g_free(filename);                           // Free the filename string
        }

        gtk_widget_destroy(dialog);                     // Destroy the dialog
        return selected_path;
    
    #endif
    }


    std::filesystem::path get_executable_path() {

    #if defined(PFF_PLATFORM_WINDOWS)

        wchar_t path[MAX_PATH];
        if (GetModuleFileNameW(NULL, path, MAX_PATH)) {
            std::filesystem::path execPath(path);
            return execPath.parent_path();
        }

    #elif defined(PFF_PLATFORM_LINUX)
        
        char path[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
        if (count != -1) {
            path[count] = '\0'; // Null-terminate the string
            std::filesystem::path execPath(path);
            return execPath.parent_path();
        }

    #endif

        std::cerr << "Error retrieving the executable path." << std::endl;
        return std::filesystem::path();
    }


}