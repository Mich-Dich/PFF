
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

    f32 random::get_f32(f32 min, f32 max) {
        std::uniform_real_distribution<f32> dist(min, max);
        return dist(engine);
    }

    bool random::get_percent(f32 percentage) { return get_f32(0.f, 1.f) > percentage; }

    f64 random::get_f64(f64 min, f64 max) {
        std::uniform_real_distribution<f64> dist(min, max);
        return dist(engine);
    }


    u32 random::get_u32(u32 min, u32 max) {
        std::uniform_int_distribution<u32> dist(min, max);
        return dist(engine);
    }


    u64 random::get_u64(u64 min, u64 max) {
        std::uniform_int_distribution<u64> dist(min, max);
        return dist(engine);
    }


    std::string util::random::get_string(const size_t length) {

        const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);

        std::string result;
        result.resize(length);
        std::generate(result.begin(), result.end(), [&]() { return charset[dist(engine)]; });
        return result;
    }


    void simple_deletion_queue::flush() {

        for (auto it = m_functions.rbegin(); it != m_functions.rend(); it++)
            (*it)();
        m_functions.clear();
    }


    bool run_program(const std::filesystem::path& path_to_exe, const std::string& cmd_args) { return run_program(path_to_exe, cmd_args.c_str()); }


    bool run_program(const std::filesystem::path& path_to_exe, const char* cmd_args) {

        bool result = false;

#ifdef PFF_PLATFORM_WINDOWS

        STARTUPINFOA startupInfo;
        PROCESS_INFORMATION processInfo;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        ZeroMemory(&processInfo, sizeof(processInfo));

        std::string cmdArguments = path_to_exe.string() + " " + cmd_args;

        // Start the program
        result = CreateProcessA(
            NULL,							// Application Name
            (LPSTR)cmdArguments.c_str(),	// Command Line Args
            NULL,							// Process Attributes
            NULL,							// Thread Attributes
            FALSE,							// Inherit Handles
            CREATE_NEW_CONSOLE,				// Creation Flags
            NULL,							// Environment
            NULL,							// Current Directory
            &startupInfo,					// Startup Info
            &processInfo					// Process Info
        );

        if (result) {
            // Close process and thread handles
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        } else
            CORE_LOG(Warn, "Unsuccessfully started process: " << path_to_exe.string().c_str());

#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC

#error Not implemented yet

#endif

        return result;
    }


    void extract_part_after_delimiter(std::string& dest, const std::string& input, const char* delimiter) {

        size_t found = input.find_last_of(delimiter);
        if (found != std::string::npos) {

            dest = input.substr(found + 1);
            return;
        }

        return; // If delimiter is not found
    }


    void extract_part_befor_delimiter(std::string& dest, const std::string& input, const char* delimiter) {

        size_t found = input.find_last_of(delimiter);
        if (found != std::string::npos) {

            dest = input.substr(0, found);
            return;
        }

        return; // If delimiter is not found
    }

    std::string extract_vaiable_name(const std::string& input) {

        std::string result = input;
        extract_part_after_delimiter(result, input, "->");
        extract_part_after_delimiter(result, result, ".");

        return result;
    }
    

    void high_precision_sleep(f32 duration_in_milliseconds) {

        static const f32 estimated_deviation = 10.0f;
        auto loc_duration_in_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::duration<f32>(duration_in_milliseconds) ).count();
        auto target_time = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(static_cast<int>(loc_duration_in_milliseconds));
        
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(loc_duration_in_milliseconds - estimated_deviation)));

        // Busy wait for the remaining time
        while (std::chrono::high_resolution_clock::now() < target_time)
            ;

        //auto actual_sleep_time = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - target_time + std::chrono::milliseconds(static_cast<int>(duration_in_milliseconds)) ).count();
        //CORE_LOG(Debug, "left over time: " << actual_sleep_time << " ms");
    }

    bool util::is_valid_project_dir(const std::filesystem::path& path) {

        if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
            return false;

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == PFF_PROJECT_EXTENTION)
                return true; // found a project file
        }

        return false;
    }

    std::filesystem::path extract_path_from_project_folder(const std::filesystem::path& full_path) {

        std::string full_path_str = full_path.string();
        std::string folder_marker = std::string(CONTENT_DIR);
        size_t pos = full_path_str.find(folder_marker);
        if (pos != std::string::npos) {

            std::string result_str = full_path_str.substr(pos);
            return std::filesystem::path(result_str);

        } else {

            CORE_LOG(Trace, "NOT FOUND");
            return {};
        }
    }

    std::filesystem::path extract_path_from_project_content_folder(const std::filesystem::path& full_path) {

        std::filesystem::path result;
        bool start_adding = false;

        for (const auto& part : full_path) {

            if (start_adding)
                result /= part;  // Add the part to the result path

            if (part == std::string(CONTENT_DIR))
                start_adding = true;
        }
        return result;
    }
    
    std::filesystem::path extract_path_from_directory(const std::filesystem::path& full_path, const std::string& directory) {

        std::filesystem::path result;
        bool start_adding = false;

        for (const auto& part : full_path) {

            if (start_adding)
                result /= part;  // Add the part to the result path

            if (part == directory)
                start_adding = true;
        }
        return result;
    }
    

    std::filesystem::path file_dialog() {

#ifdef PFF_PLATFORM_WINDOWS

        OPENFILENAME ofn;       // common dialog box structure
        char szFile[260];       // buffer for file name

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = (LPWSTR)szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = _T("All Files\0*.*\0Text Files\0*.TXT\0");
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // Display the Open File dialog box 
        if (GetOpenFileName(&ofn) == TRUE)
            return std::filesystem::path(ofn.lpstrFile);

        return std::filesystem::path();

#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC

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


    system_time get_system_time() {

        system_time loc_system_time{};

#ifdef PFF_PLATFORM_WINDOWS

        SYSTEMTIME win_time;
        GetLocalTime(&win_time);
        loc_system_time.year =              static_cast<u16>(win_time.wYear);
        loc_system_time.month =             static_cast<u8>(win_time.wMonth);
        loc_system_time.day =               static_cast<u8>(win_time.wDay);
        loc_system_time.day_of_week =       static_cast<u8>(win_time.wDayOfWeek);
        loc_system_time.hour =              static_cast<u8>(win_time.wHour);
        loc_system_time.minute =            static_cast<u8>(win_time.wMinute);
        loc_system_time.secund =            static_cast<u8>(win_time.wSecond);
        loc_system_time.millisecends =      static_cast<u16>(win_time.wMilliseconds);

#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC

        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm* ptm = localtime(&tv.tv_sec);
        loc_system_time.year =              static_cast<u16>(ptm->tm_year + 1900);
        loc_system_time.month =             static_cast<u8>(ptm->tm_mon + 1);
        loc_system_time.day =               static_cast<u8>(ptm->tm_mday);
        loc_system_time.day_of_week =       static_cast<u8>(ptm->tm_wday);
        loc_system_time.hour =              static_cast<u8>(ptm->tm_hour);
        loc_system_time.minute =            static_cast<u8>(ptm->tm_min);
        loc_system_time.secund =            static_cast<u8>(ptm->tm_sec);
        loc_system_time.millisecends =      static_cast<u16>(tv.tv_usec / 1000);

#endif
        return loc_system_time;
    }

    void util::cancel_timer(timer& timer) {

        timer.shared_state->first = true;
        timer.shared_state->second.notify_one();
        if (timer.future.valid()) {
            timer.future.wait();
        }
    }

    std::string add_spaces(const u32 multiple_of_indenting_spaces, u32 num_of_indenting_spaces) {

        if (multiple_of_indenting_spaces == 0)
            return "";

        return std::string(multiple_of_indenting_spaces * num_of_indenting_spaces, ' ');
    }


    u32 measure_indentation(const std::string& str, u32 num_of_indenting_spaces) {

        u32 count = 0;
        for (char ch : str) {
            if (ch == ' ')
                count++;
            else
                break; // Stop counting on non-space characters
        }

        return count / num_of_indenting_spaces;
    }

    int count_lines(const char* text) {

        if (text[0] == '\0')
            return 1;

        int line_count = 0;
        for (int i = 0; i < 256; ++i) {
            if (text[i] == '\0')
                break;

            if (text[i] == '\n')
                ++line_count;
        }

        // If the last character is not a newline and the string is not empty, count the last line
        if (text[0] != '\0' && text[255] != '\n')
            ++line_count;

        return line_count;
    }

}