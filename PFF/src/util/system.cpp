
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
    #include <QApplication>
    #include <QFileDialog>
    #include <QString>
    #include <sys/time.h>
    #include <ctime>
    #include <limits.h>
#else
    #error "OS not supported"
#endif

#include "system.h"


namespace PFF::util {

    void open_console(const char* title, const bool enable_anci_codes) {
    
#if defined(PFF_PLATFORM_WINDOWS)
        
        AllocConsole();
        FILE* p_file;
        freopen_s(&p_file, "CONOUT$", "w", stdout);
        freopen_s(&p_file, "CONOUT$", "w", stderr);
        freopen_s(&p_file, "CONIN$", "r", stdin);

        std::cout.clear();                                      // Clear the error state for each of the C++ standard stream objects
        std::cerr.clear();
        std::cin.clear();

        SetConsoleTitleA(title);

        if (!enable_anci_codes)
            return;

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

#elif defined(PFF_PLATFORM_LINUX)

        // On Linux, the standard streams are typically already connected to the terminal so we don't need to allocate a new console like in Windows
        std::cout.clear();
        std::cerr.clear();
        std::cin.clear();
        
        if (isatty(STDOUT_FILENO)) {                                // Set terminal title if we're running in a terminal
            std::cout << "\033]0;" << title << "\007";
            std::cout.flush();
        }
        
        // [enable_anci_codes] - ANSI codes are typically enabled by default on Linux terminals so we don't need to do anything special for enable_anci_codes
#endif
    }


    //
    bool run_program(const std::filesystem::path& path_to_exe, const std::string& cmd_args, bool open_console) { return run_program(path_to_exe, cmd_args.c_str(), open_console); }

    //
    bool run_program(const std::filesystem::path& path_to_exe, const char* cmd_args, bool open_console) {

        //LOG(Trace, "executing program at [" << path_to_exe.generic_string() << "]");

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
            LOG(Error, "Unsuccessfully started process: " << path_to_exe.generic_string());

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
        //LOG(Debug, "left over time: " << actual_sleep_time << " ms");
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
        loc_system_time.millisecend = static_cast<u16>(win_time.wMilliseconds);

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
        loc_system_time.millisecend = static_cast<u16>(tv.tv_usec / 1000);

#endif
        return loc_system_time;
    }

#if defined(PFF_PLATFORM_LINUX)                                     // QT related functions (Linux only)

    namespace {
        std::unique_ptr<QApplication> qt_app;
        int qt_argc = 1;
        char qt_argv0[] = "PFF_editor";  // Permanent storage for argv[0]
        char* qt_argv[] = {qt_argv0, nullptr};
    }

    void qt_message_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
        
        // Map Qt message type to your severity levels
        PFF::logger::severity sev = PFF::logger::severity::Debug;
        switch(type) {
            case QtDebugMsg:    sev = PFF::logger::severity::Debug; break;
            case QtInfoMsg:     sev = PFF::logger::severity::Info; break;
            case QtWarningMsg:  sev = PFF::logger::severity::Warn; break;
            case QtCriticalMsg: sev = PFF::logger::severity::Error; break;
            case QtFatalMsg:    sev = PFF::logger::severity::Fatal; break;
        }
        
        // Extract context information
        const char* file            = context.file ? context.file : "none";
        const char* function        = context.function ? context.function : "none";
        int line                    = context.line;
        std::thread::id threadId    = std::this_thread::get_id();
        std::string message         = msg.toStdString();
        PFF::logger::log_msg(sev, file, function, line, threadId, std::move(message));
    }
    
    void init_qt() {
            
        LOG(Trace, "Initiating QT");
    
        qInstallMessageHandler(qt_message_handler);
        
        if (!qt_app) {
            QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
            qt_app = std::make_unique<QApplication>(qt_argc, qt_argv);
        }
    }
    
    void shutdown_qt() { qt_app.reset(); }

#endif

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
   
         if (!qt_app) {
            LOG(Error, "QApplication not initialized!");
            return {};
        }

        QString filterString;
        for (auto& filter : filters) {                                                                                                  // Prepare the filter string for QFileDialog
            
            // Replace semicolons with spaces
            std::string buffer = filter.second;
            size_t pos = 0;
            while ((pos = buffer.find(';', pos)) != std::string::npos) {                                                                // Replace ';' with ' ' 
                buffer.replace(pos, 1, " ");
                pos += 1;
            }
            
            filterString += QString::fromStdString(filter.first) + " (" + QString::fromStdString(buffer) + ");;";
        }
        filterString.chop(2); // Remove the last ";;"

        QString fileName = QFileDialog::getOpenFileName(nullptr, QString::fromUtf8(title.data()), QString(), filterString);             // Open the file dialog
        return std::filesystem::path(fileName.toStdString());

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