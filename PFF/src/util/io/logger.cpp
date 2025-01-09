
#include "util/pffpch.h"

#include "util/system.h"
#include "util/io/io.h"

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

    static int enable_ANSI_escape_codes() {

#if defined PFF_PLATFORM_WINDOWS

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

#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC

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
}


#define PROJECT_FOLDER                  "PFF"

#define SHORTEN_FILE_PATH(text)         (strstr(text, PROJECT_FOLDER) ? strstr(text, PROJECT_FOLDER) + strlen(PROJECT_FOLDER) + 1 : text)
#define SHORT_FILE(text)                (strrchr(text, "\\") ? strrchr(text, "\\") + 1 : text)
#define SHORTEN_FUNC_NAME(text)         (strstr(text, "::") ? strstr(text, "::") + 2 : text)

static const char* get_filename(const char* filepath) {

    const char* filename = std::strrchr(filepath, '\\');
    if (filename == nullptr) 
        filename = std::strrchr(filepath, '/');

    if (filename == nullptr) 
        return filepath;  // No path separator found, return the whole string

    return filename + 1;  // Skip the path separator
}

namespace PFF::logger {

    static const char* SeperatorStringBig = "====================================================================================================================";
    static const char* SeperatorStringSmall = "--------------------------------------------------------------------------------------------------------------------";
    static const char* SeverityNames[log_msg_severity::NUM_SEVERITIES]{ "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
    static const char* LogFileName = "logFile.txt";
    static const char* LogCoreFileName = "logFileCORE.txt";
    static std::string LogMessageFormat = "[$B$T:$J$E] [$B$L$X - $A - $F:$G$E] $C$Z";
    static std::string LogMessageFormat_BACKUP = "$B[$T] $L$E - $C";
    static const char* displayed_Path_Start = "vulkan_test\\";                          // MAYBE: remove
    static const char* displayed_FuncName_Start = "PFF::";                              // MAYBE: remove
    static const char* ConsoleRESET = "\x1b[0m";
    static int Buffer_Level;
    static int LegLevelToBuffer = 3;
    static const char* ConsoleColorTable[log_msg_severity::NUM_SEVERITIES] = {
        "\x1b[38;5;246m",           // Trace: Gray
        "\x1b[94m",           // Debug: Blue
        "\x1b[92m",           // Info: Green
        "\x1b[33m",           // Warn: Yellow
        "\x1b[31m",           // Error: Red
        "\x1b[41m\x1b[30m",   // Fatal: Red Background
    };

    // [$B$T:$J$E] [$B$L$X $I - $P:$G$E] $C$Z

    // ================================================================= public functions =================================================================

    bool init(const std::string& format) {

        enable_ANSI_escape_codes();

        std::string file_dir = "./logs";
        io::create_directory(file_dir);

        LogCoreFileName = (file_dir + "/engine.log").c_str();
        LogFileName = (file_dir + "/project.log").c_str();

        set_format(format);

        std::ostringstream Init_Message;
        Init_Message.flush();

        system_time loc_system_time = util::get_system_time();
        Init_Message << "[" << SETW(4) << loc_system_time.year
            << "/" << SETW(2) << loc_system_time.month
            << "/" << SETW(2) << loc_system_time.day
            << " - " << SETW(2) << loc_system_time.hour
            << ":" << SETW(2) << loc_system_time.minute
            << ":" << SETW(2) << loc_system_time.secund << "]"
            << "  Log initialized" << std::endl

            << "   Inital Log Format: '" << format << "'" << std::endl << "   Enabled Log Levels: ";

        static const char* loc_level_str[6] = { "Fatal", " + Error", " + Warn", " + Info", " + Debug", " + Trace" };
        for (int x = 0; x < LOG_LEVEL_ENABLED + 2; x++)
            Init_Message << loc_level_str[x];

        Init_Message << std::endl << SeperatorStringBig;

        // Write the content to a file
        std::ofstream outputFile(LogFileName);
        if (!outputFile.is_open()) {
            std::cerr << "Error: Unable to open the file for writing." << std::endl;
            return false;
        }

        outputFile << Init_Message.str() << std::endl;
        outputFile.close();

        CORE_LOG(Trace, "Subsystem [Logger] initialized");
        CORE_LOG_SEPERATOR_BIG;
        return true;
    }

    // change Format for all following messages
    void set_format(const std::string& format) {

        LogMessageFormat_BACKUP = LogMessageFormat;
        LogMessageFormat = format;
    }

    // Use previous Format
    void use_format_backup() {

        std::string m_buffer = LogMessageFormat;
        LogMessageFormat = LogMessageFormat_BACKUP;
        LogMessageFormat_BACKUP = m_buffer;
    }

    // deside with messages should be bufferd and witch are written to file instantly
    void set_buffer_Level(int newLevel) {

        if (newLevel > 0 && newLevel < 5)
            LegLevelToBuffer = newLevel;
    }


    log_message::log_message(log_msg_severity severity, const char* fileName, const char* funcName, int line) :
        m_Severity(severity), m_FileName(fileName), m_FuncName(funcName), m_Line(line) {}

    log_message::~log_message() {

        std::string loc_string = str();
        const char* message = loc_string.c_str();
        if (loc_string.empty())
            return;

        system_time loc_system_time = util::get_system_time();

        // Create Buffer vars
        std::ostringstream Format_Filled;
        Format_Filled.flush();
        char Format_Command;

        // Loop over Format string and build Final Message
        int FormatLen = static_cast<int>(LogMessageFormat.length());
        for (int x = 0; x < FormatLen; x++) {

            if (LogMessageFormat[x] == '$' && x + 1 < FormatLen) {

                Format_Command = LogMessageFormat[x + 1];
                switch (Format_Command) {

                // ------------------------------------  Basic Info  -------------------------------------------------------------------------------
                case 'B':   Format_Filled << ConsoleColorTable[m_Severity]; break;                                                                                                          // Color Start
                case 'E':   Format_Filled << ConsoleRESET; break;                                                                                                                           // Color End
                case 'C':   Format_Filled << message; break;                                                                                                                                // input text (message)
                case 'L':   Format_Filled << SeverityNames[m_Severity]; break;                                                                                                              // Log Level
                case 'X':   if (m_Severity == log_msg_severity::Info || m_Severity == log_msg_severity::Warn) { Format_Filled << " "; } break;                                              // Alignment
                case 'Z':   Format_Filled << std::endl; break;                                                                                                                              // Alignment

                // ------------------------------------  Source  -------------------------------------------------------------------------------
                case 'F':   Format_Filled << m_FuncName; break;                                                                                                                             // Function Name
                case 'P':   Format_Filled << SHORTEN_FUNC_NAME(m_FuncName); break;                                                                                                          // Function Name
                case 'A':   Format_Filled << m_FileName; break;                                                                                                                             // File Name
                case 'K':   Format_Filled << SHORTEN_FILE_PATH(m_FileName); break;                                                                                                          // Shortend File Name
                case 'I':   Format_Filled << get_filename(m_FileName); break;                                                                                                               // Only File Name
                case 'G':   Format_Filled << m_Line; break;                                                                                                                                 // Line

                // ------------------------------------  Time  -------------------------------------------------------------------------------
                case 'T':   Format_Filled << SETW(2) << (u16)loc_system_time.hour << ":" << SETW(2) << (u16)loc_system_time.minute << ":" << SETW(2) << (u16)loc_system_time.secund; break; // Clock hh:mm:ss
                case 'H':   Format_Filled << SETW(2) << (u16)loc_system_time.hour; break;                                                                                                   // Clock secunde
                case 'M':   Format_Filled << SETW(2) << (u16)loc_system_time.minute; break;                                                                                                 // Clock minute
                case 'S':   Format_Filled << SETW(2) << (u16)loc_system_time.secund; break;                                                                                                 // Clock second
                case 'J':   Format_Filled << SETW(3) << (u16)loc_system_time.millisecends; break;                                                                                           // Clock millisec.

                // ------------------------------------  Date  -------------------------------------------------------------------------------
                case 'N':   Format_Filled << SETW(4) << (u16)loc_system_time.year << "/" << SETW(2) << (u16)loc_system_time.month << "/" << SETW(2) << (u16)loc_system_time.day; break;     // Data yyyy/mm/dd
                case 'Y':   Format_Filled << SETW(4) << (u16)loc_system_time.year; break;                                                                                                   // Year
                case 'O':   Format_Filled << SETW(2) << (u16)loc_system_time.month; break;                                                                                                  // Month
                case 'D':   Format_Filled << SETW(2) << (u16)loc_system_time.day; break;                                                                                                    // Day

                // ------------------------------------  Default  -------------------------------------------------------------------------------
                default: break;
                }

                x++;
            }

            else
                Format_Filled << LogMessageFormat[x];

        }

        std::cout << Format_Filled.str();

        // Write the content to a file   !! NO BUFFER YET !!
        std::ofstream outputFile(LogFileName, std::ios::app);
        if (outputFile.is_open()) {

            outputFile << Format_Filled.str();
            outputFile.close();
        }
    }

    
}
