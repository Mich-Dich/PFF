
#include "util/pffpch.h"

#include "util/system.h"
#include "util/io/io.h"
#include "util/system.h"

#include <list>
#include <map>
#include <type_traits>
#include <stdio.h>
#include <cstdarg>
#include <fstream>
#include <iomanip>

#include "logger.h"


#define SETW(width)             std::setw(width) << std::setfill('0')



#if defined PFF_PLATFORM_WINDOWS
    #include <Windows.h>
#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC
    #include <iostream>
    #include <unistd.h>
    #include <termios.h>
#endif


namespace PFF::logger {

    #define SETW(width)                 std::setw(width) << std::setfill('0')
    #define INTERNAL_LOG(message)       { std::ostringstream oss{}; oss << message; log_string(std::move(oss.str())); }

    static bool                         is_init = false;
    static bool                         write_log_to_console = false;
    static std::string                  format_current = "";
    static std::string                  format_prev = "";

    static severity                     severity_level_buffering_threshhold = severity::Trace;
    static size_t                       buffer_size = 1024;
    static std::string                  buffered_messages{};

    const std::string_view              severity_names[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    const std::string_view              console_rest = "\x1b[0m";
    const std::string_view              console_color_table[] = {
        "\x1b[38;5;246m",                   // Trace: Gray
        "\x1b[94m",                         // Debug: Blue
        "\x1b[92m",                         // Info: Green
        "\x1b[33m",                         // Warn: Yellow
        "\x1b[31m",                         // Error: Red
        "\x1b[41m\x1b[30m",                 // Fatal: Red Background
    };

    static std::filesystem::path        main_log_dir = "";
    static std::filesystem::path        main_log_file_path = "";
    static std::ofstream                main_file{};

    inline void log_string(std::string&& log_str, const bool force_direct_logging = true) {

        if (!(force_direct_logging || (buffered_messages.capacity() - buffered_messages.size()) <= log_str.size())) {

            buffered_messages.append(log_str);
            return;
        }
        
        main_file = std::ofstream(main_log_file_path, std::ios::app);
        if (!main_file.is_open()) {
            std::cerr << "Failed to open main log files" << std::endl;
            std::quick_exit(1);
        }

        main_file << buffered_messages << log_str;

        main_file.close();

        if (write_log_to_console) 
            std::cout << buffered_messages << log_str;

        buffered_messages.clear();
    }

    inline const char* get_filename(const char* filepath) {

        const char* filename = std::strrchr(filepath, '\\');
        if (filename == nullptr)
            filename = std::strrchr(filepath, '/');

        if (filename == nullptr)
            return filepath;  // No path separator found, return the whole string

        return filename + 1;  // Skip the path separator
    }

    static int enable_ANSI_escape_codes() {

        #if defined(PFF_PLATFORM_WINDOWS)
        
                // Enable ANSI escape codes
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                if (hOut == INVALID_HANDLE_VALUE) {
                    std::cerr << "Failed to get handle to console." << std::endl;
                    return -1;
                }
        
                DWORD dwMode = 0;
                if (!GetConsoleMode(hOut, &dwMode)) {
                    std::cerr << "Failed to get console mode." << std::endl;
                    return -1;
                }
        
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                if (!SetConsoleMode(hOut, dwMode)) {
                    std::cerr << "Failed to set console mode." << std::endl;
                    return -1;
                }
        
                return 0;
        
        #elif defined(PFF_PLATFORM_LINUX) || defined(PFF_PLATFORM_MAC)
        
                struct termios term;
                if (tcgetattr(STDOUT_FILENO, &term) == -1) {
                    std::cerr << "Failed to get terminal attributes." << std::endl;
                    return -1;
                }
        
                term.c_lflag &= ~(ICANON | ECHO);       // Disable canonical mode and echo
                term.c_oflag |= OPOST;                  // Enable output processing
                if (tcsetattr(STDOUT_FILENO, TCSANOW, &term) == -1) {
                    std::cerr << "Failed to set terminal attributes." << std::endl;
                    return -1;
                }
        
                return 0;
        #endif
        
    }

    // ========================================================================================================================
    // init / shutdown
    // ========================================================================================================================

    bool init(const std::string& format, const bool log_to_console, const std::filesystem::path log_dir, const std::string& main_log_file_name, const bool use_append_mode) {

        if (is_init) {
            std::cerr << "Tryed to init logger system multiple times" << std::endl;
            std::quick_exit(1);
        }

        format_current = format;
        format_prev = format;
        write_log_to_console = log_to_console;

        if (!std::filesystem::is_directory(log_dir))
            if (!std::filesystem::create_directory(log_dir)) {
                std::cerr << "Failed to create the directory for log files" << std::endl;
                std::quick_exit(1);
            }

        main_log_dir = log_dir;
        main_log_file_path = log_dir / main_log_file_name;

        main_file = std::ofstream(main_log_file_path, (use_append_mode) ? std::ios::app : std::ios::out );
        if (!main_file.is_open()) {
            std::cerr << "Failed to open main log files" << std::endl;
            std::quick_exit(1);
        }

        main_file << "\n========================================================\n";

        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        main_file << "Log initalized at [" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "]\n";

        main_file << "========================================================\n";
        main_file.close();
        buffered_messages.reserve(buffer_size);

        is_init = true;
        return true;
    }

    void shutdown() {

        if (!is_init) {
            std::cerr << "Tried to shutdown logger bevor initilization" << std::endl;
            std::quick_exit(1);
        }

        if ( !buffered_messages.empty()) {

            main_file = std::ofstream(main_log_file_path, std::ios::app);
            if (!main_file.is_open()) {
                std::cerr << "Failed to open main log files" << std::endl;
                std::quick_exit(1);
            }

            main_file << buffered_messages;

            if (write_log_to_console)
                std::cout << buffered_messages;

        }

        if (main_file.is_open())
            main_file.close();

        is_init = false;
    }    

    // ========================================================================================================================
    // settings
    // ========================================================================================================================

    void set_format(const std::string& new_format) {

        format_prev = format_current;
        format_current = std::move(new_format);
        INTERNAL_LOG("[LOGGER] changed log-format. From[" << format_prev << "] to [" << format_current << "]\n");
    }

    void use_previous_format() {

        const std::string buffer = format_current;
        format_current = format_prev;
        format_prev = buffer;
        INTERNAL_LOG("[LOGGER] Changed to previous log-format. From [" << format_prev << "] to [" << format_current << "]\n");
    }

    const std::string get_format() { return format_current; }

    void set_buffer_threshhold(const severity new_threshhold) {

        severity_level_buffering_threshhold = static_cast<severity>(std::min(static_cast<u8>(new_threshhold), static_cast<u8>(severity::Error)));

        INTERNAL_LOG("[LOGGER] Changed buffering threshhold to [" << severity_names[static_cast<u8>(severity_level_buffering_threshhold)] << "]\n");
    }

    void set_buffer_size(const size_t new_size) {

        INTERNAL_LOG("[LOGGER] Changed buffer size to [" << new_size << "]\n");
        buffer_size = new_size;

        if (is_init && buffered_messages.size() >= buffer_size) {

            main_file = std::ofstream(main_log_file_path, std::ios::app);
            if (!main_file.is_open()) {
                std::cerr << "Failed to open main log file" << std::endl;
                std::quick_exit(1);
            }
            main_file << buffered_messages;
            main_file.close();

            if (write_log_to_console)
                std::cout << buffered_messages;

            buffered_messages.clear();
        }

        buffered_messages.shrink_to_fit();
        buffered_messages.reserve(buffer_size);
    }

    // ========================================================================================================================
    // handle message
    // ========================================================================================================================

    void log_msg(const severity msg_sev, const char* file_name, const char* function_name, const int line, std::string&& message) {

#define SHORTEN_FUNC_NAME(text)                                 (strstr(text, "::") ? strstr(text, "::") + 2 : text)

        if (message.empty())
            return;

        // create helper vars
        std::ostringstream format_filled{};
        format_filled.flush();
        char format_command{};
        system_time loc_sys_time = util::get_system_time();

        // loop over format string and build final message
        int format_length = static_cast<int>(format_current.length());
        for (int x = 0; x < format_length; x++) {

            if (format_current[x] == '$' && x+1 < format_length) {          // detected a format specifier prefix

                format_command = format_current[x + 1];
                switch (format_command) {

                // ------------------------ Basic info ------------------------
                case 'B': format_filled << console_color_table[(u8)msg_sev]; break;                                         // Color start
                case 'E': format_filled << console_rest; break;                                                             // Color end
                case 'C': format_filled << message; break;                                                                  // input text (message)
                case 'L': format_filled << severity_names[(u8)msg_sev]; break;                                              // log severity
                case 'X': if(msg_sev == severity::Info || msg_sev == severity::Warn) {format_filled << " "; } break;        // alignment
                case 'Z': format_filled << "\n"; break;                                                                     // line breake
                
                // ------------------------ Basic info ------------------------
                case 'F': format_filled << function_name; break;                                                            // function name
                case 'P': format_filled << SHORTEN_FUNC_NAME(function_name); break;                                         // short function name
                case 'A': format_filled << file_name; break;                                                                // file name
                case 'I': format_filled << get_filename(file_name); break;                                                  // short file name
                case 'G': format_filled << line; break;                                                                     // line
                
                // ------------------------ time ------------------------
                case 'T': format_filled << SETW(2) << (u16)loc_sys_time.hour << ":" << SETW(2) << (u16)loc_sys_time.minute << ":" << SETW(2) << (u16)loc_sys_time.secund; break;        // formated time
                case 'H': format_filled << SETW(2) << (u16)loc_sys_time.hour; break;                                                                                                    // hour
                case 'M': format_filled << SETW(2) << (u16)loc_sys_time.minute; break;                                                                                                  // minute
                case 'S': format_filled << SETW(2) << (u16)loc_sys_time.secund; break;                                                                                                  // second
                case 'J': format_filled << SETW(3) << (u16)loc_sys_time.millisecends; break;                                                                                            // miliseond

                // ------------------------ data ------------------------
                case 'N': format_filled << SETW(4) << (u16)loc_sys_time.year << "/" << SETW(2) << (u16)loc_sys_time.month <<"/" << SETW(2) << (u16)loc_sys_time.day; break;             // data yy/mm/dd
                case 'Y': format_filled << SETW(4) << (u16)loc_sys_time.year; break;                                                                                                    // year
                case 'O': format_filled << SETW(2) << (u16)loc_sys_time.month; break;                                                                                                   // month
                case 'D': format_filled << SETW(2) << (u16)loc_sys_time.day; break;                                                                                                     // day

                default: break;
                }

                x++;
            }
            
            else
                format_filled << format_current[x];
        }

        log_string(std::move(format_filled.str()), static_cast<u8>(msg_sev) >= static_cast<u8>(severity_level_buffering_threshhold));
    }
    
}
