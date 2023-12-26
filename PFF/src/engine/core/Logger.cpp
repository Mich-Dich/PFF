
//#include "glpch.h"
    #include <iostream>
    #include <memory>
    #include <utility>
    #include <algorithm>
    #include <functional>

    #include <string>
    #include <sstream>
    #include <vector>
    #include <unordered_map>
    #include <unordered_set>

    // from logger.h
    #include <list>
    #include <map>
    #include <type_traits>

    // from logger.cpp
    #include <stdio.h>
    #include <cstdarg>
    #include <fstream>
    #include <iomanip>
    #include <Windows.h>

#include "Logger.h"


#define SHORTEN_FILE_PATH(file)         (strstr(file, PROJECT_FOLDER) ? strstr(file, PROJECT_FOLDER) + strlen(PROJECT_FOLDER) + 1 : file)

#define SHORTEN_FUNC_NAME(name)         (strrchr(name, '::') ? strrchr(name, '::') + 1 : name)


namespace APP_NAMESPACE {

    namespace Logger {

        static const char* SeverityNames[LogMsgSeverity::NUM_SEVERITIES]{ "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
        static const char* LogFileName = "logFile.txt";
        static const char* LogCoreFileName = "logFileCORE.txt";
        static const char* LogMessageFormat = "[$B$T:$J$E] [$B$L$X - $A - $F:$G$E] $C$Z";
        static const char* LogMessageFormat_BACKUP = "$B[$T] $L$E - $C";
        static const char* displayed_Path_Start = "VulkanTest\\";
        static const char* displayed_FuncName_Start = "Gluttony::";
        static const char* ConsoleRESET = "\x1b[97m\x1b[40m";
        static int Buffer_Level;
        static int LegLevelToBuffer = 3;

        static const char* ConsoleColorTable[LogMsgSeverity::NUM_SEVERITIES] = {
          "\x1b[90m",           // Gray
          "\x1b[94m",           // Blue
          "\x1b[92m",           // Green
          "\x1b[33m",           // Yellow
          "\x1b[31m",           // Red
          "\x1b[41m\x1b[30m",   // Red Background
        };

        int Init(const char* LogCoreFile, const char* LogFile, const char* Format) {

            LogCoreFileName = LogCoreFile;
            LogFileName = LogFile;
            Set_Format(Format);

            std::ostringstream Init_Message;
            Init_Message.flush();

            SYSTEMTIME TimeLoc;
            GetLocalTime(&TimeLoc);

            Init_Message << "[" << std::setw(4) << std::setfill('0') << TimeLoc.wYear
                << "/" << std::setw(2) << std::setfill('0') << TimeLoc.wMonth
                << "/" << std::setw(2) << std::setfill('0') << TimeLoc.wDay
                << " - " << std::setw(2) << std::setfill('0') << TimeLoc.wHour
                << ":" << std::setw(2) << std::setfill('0') << TimeLoc.wMinute
                << ":" << std::setw(2) << std::setfill('0') << TimeLoc.wSecond << "]"
                << "  Log initialized" << std::endl

                << "   Inital Log Format: '" << Format << "'" << std::endl << "   Enabled Log Levels: ";

            static const char* loc_level_str[6] = { "Fatal", " + Error", " + Warn", " + Info", " + Debug", " + Trace" };
            for (int x = 0; x < LOG_LEVEL_ENABLED + 2; x++)
                Init_Message << loc_level_str[x];

            Init_Message << std::endl << SeperatorStringBig;

            // Write the content to a file
            std::ofstream outputFile(LogFileName);
            if (outputFile.is_open()) {
                outputFile << Init_Message.str() << std::endl;
                outputFile.close();
            } else {
                std::cerr << "Error: Unable to open the file for writing." << std::endl;
                return -1;
            }

            CORE_LOG(Trace, "Subsystem [Logger] initialized");
            //GL_CORE_LOG_SEPERATOR_BIG;
            return 0;
        }

        // change Format for all following messages
        void Set_Format(const char* newFormat) {

            LogMessageFormat_BACKUP = LogMessageFormat;
            LogMessageFormat = newFormat;
        }

        // Use previous Format
        void Use_Format_Backup() {

            const char* buffer = LogMessageFormat;
            LogMessageFormat = LogMessageFormat_BACKUP;
            LogMessageFormat_BACKUP = buffer;
        }

        // deside with messages should be bufferd and witch are written to file instantly
        void set_buffer_Level(int newLevel) {

            if (newLevel > 0 && newLevel < 5) {
                LegLevelToBuffer = newLevel;
            }

        }

        void LogMsg(LogMsgSeverity level, const char* fileName, const char* funcName, int line, const char* message) {

            if (strlen(message) == 0)
                return;

            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            // Get the current local system time
            SYSTEMTIME TimeLoc;
            GetLocalTime(&TimeLoc);

            // Create Buffer vars
            std::ostringstream Format_Filled;
            Format_Filled.flush();
            char Format_Command;


            // Loop over Format string and build Final Message
            int FormatLen = static_cast<int>(strlen(LogMessageFormat));
            for (int x = 0; x < FormatLen; x++) {

                if (LogMessageFormat[x] == '$' && x + 1 < FormatLen) {

                    Format_Command = LogMessageFormat[x + 1];
                    switch (Format_Command) {

                        // ------------------------------------  Basic Info  -------------------------------------------------------------------------------
                        // Color Start
                    case 'B':
                        Format_Filled << ConsoleColorTable[level];
                        break;

                        // Color End
                    case 'E':
                        Format_Filled << ConsoleRESET;
                        break;

                        // input text (message)
                    case 'C':
                        Format_Filled << message;
                        break;

                        // Log Level
                    case 'L':
                        Format_Filled << SeverityNames[level];
                        break;

                        // Alignment
                    case 'X':
                        if (level == LogMsgSeverity::Info || level == LogMsgSeverity::Warn) { Format_Filled << " "; }
                        break;

                        // Alignment
                    case 'Z':
                        Format_Filled << std::endl;
                        break;

                        // ------------------------------------  Source  -------------------------------------------------------------------------------
                        // Function Name
                    case 'F':
                        Format_Filled << funcName;
                        break;
                        
                        // Function Name
                    case 'P':
                        Format_Filled << SHORTEN_FUNC_NAME(funcName);
                        break;

                        // File Name
                    case 'A':
                        Format_Filled << fileName;
                        break;

                        // Shortend File Name
                    case 'K':
                        Format_Filled << SHORTEN_FILE_PATH(fileName);
                        break;

                        // Only File Name
                    case 'I':
                        Format_Filled << extractFileName(fileName);
                        break;

                        // Line
                    case 'G':
                        Format_Filled << line;
                        break;

                        // ------------------------------------  Time  -------------------------------------------------------------------------------
                        // Clock hh:mm:ss
                    case 'T':
                        Format_Filled << std::setw(2) << std::setfill('0') << TimeLoc.wHour
                            << ":" << std::setw(2) << std::setfill('0') << TimeLoc.wMinute
                            << ":" << std::setw(2) << std::setfill('0') << TimeLoc.wSecond;
                        break;

                        // Clock secunde
                    case 'H':
                        Format_Filled << std::setw(2) << std::setfill('0') << TimeLoc.wHour;
                        break;

                        // Clock minute
                    case 'M':
                        Format_Filled << std::setw(2) << std::setfill('0') << TimeLoc.wMinute;
                        break;

                        // Clock second
                    case 'S':
                        Format_Filled << std::setw(2) << std::setfill('0') << TimeLoc.wSecond;

                        break;

                        // Clock millisec.
                    case 'J':
                        Format_Filled << std::setw(3) << std::setfill('0') << TimeLoc.wMilliseconds;

                        break;

                        // ------------------------------------  Date  -------------------------------------------------------------------------------
                        // Data yyyy/mm/dd
                    case 'N':
                        Format_Filled << std::setw(4) << std::setfill('0') << TimeLoc.wYear
                            << "/" << std::setw(2) << std::setfill('0') << TimeLoc.wMonth
                            << "/" << std::setw(2) << std::setfill('0') << TimeLoc.wDay;
                        break;

                        // Year
                    case 'Y':
                        Format_Filled << std::setw(4) << std::setfill('0') << TimeLoc.wYear;
                        break;

                        // Month
                    case 'O':
                        Format_Filled << std::setw(2) << std::setfill('0') << TimeLoc.wMonth;
                        break;

                        // Day
                    case 'D':
                        Format_Filled << std::setw(2) << std::setfill('0') << TimeLoc.wDay;
                        break;

                        // ------------------------------------  Default  -------------------------------------------------------------------------------
                    default:
                        break;
                    }

                    x++;
                }

                else {

                    Format_Filled << LogMessageFormat[x];
                }
            }

            std::cout << Format_Filled.str();


            // Write the content to a file
            std::ofstream outputFile(LogFileName, std::ios::app);
            if (outputFile.is_open()) {

                outputFile << Format_Filled.str();
                outputFile.close();
            }
        }

        //
        const char* extractFileName(const char* filePath) {
            const char* fileName = std::strrchr(filePath, '/'); // For Unix-like paths
            if (!fileName) {
                fileName = std::strrchr(filePath, '\\'); // For Windows paths
                if (!fileName) {
                    fileName = filePath;
                } else {
                    fileName++; // Move past the backslash
                }
            } else {
                fileName++; // Move past the slash
            }

            return fileName;
        }


        LogMessage::LogMessage(LogMsgSeverity severity, const char* fileName, const char* funcName, int line) :
            m_Severity(severity), m_FileName(fileName), m_FuncName(funcName), m_Line(line) {}

        LogMessage::~LogMessage() {

            LogMsg(m_Severity, m_FileName, m_FuncName, m_Line, str().c_str());
        }
    }
}

