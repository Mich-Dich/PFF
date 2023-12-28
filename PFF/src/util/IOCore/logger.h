#pragma once

#include "engine/core.h"

#include <errno.h>
#include <stdint.h>


#include "util/dll_interface.h"

#if defined(_WIN32) || defined(__CYGWIN__)
    #include <Windows.h>
    #define CL_THREAD_ID            (uintptr_t)GetCurrentThreadId()

    const char* CL_Get_Error_String(int errnum);
    #define ERROR_STR               CL_Get_Error_String(errno)

#else                                           // APPLE IS NOT SUPORTED
    #include <pthread.h>
    #define CL_THREAD_ID            (uintptr_t)pthread_self()
    #define ERROR_STR               strerror(errno)
#endif



// This enables the compilation of various logging levels (FATAL & ERROR are always on)
//  0    =>   FATAL + ERROR
//  1    =>   FATAL + ERROR + WARN
//  2    =>   FATAL + ERROR + WARN + INFO
//  3    =>   FATAL + ERROR + WARN + INFO + DEBUG
//  4    =>   FATAL + ERROR + WARN + INFO + DEBUG + TRACE
#define LOG_LEVEL_ENABLED 4
#define MAX_MESSAGE_SIZE        2048
#define MAX_BUFFERED_MESSAGES   20

#define VALIDATE_ENABLED
#define ASSERT_ENABLED

typedef enum {
    Fatal = 0,
    Error = 1,
    Warn = 2,
    Info = 3,
    Debug = 4,
    Trace = 5,
    LL_MAX_NUM = 6
} log_level;

struct CL_Time_Info {
    int TI_year;
    int TI_month;
    int TI_day;
    int TI_hour;
    int TI_minute;
    int TI_sec;
    int TI_nSec;
};

#define CL_TRUE 1
#define CL_FALSE 0


// ------------------------------------------------------------------------------ Main Functions ------------------------------------------------------------------------------

/*  Formatting the LogMessages can be customized with the following tags
    to format all following Log Messages use: logger_set_format(char* format);
    e.g. logger_set_format("$B[$T] $L [$F]  $C$E")  or logger_set_format("$BTime:[$M $S] $L $E ==> $C")
    
    $T		Time				hh:mm:ss
    $H		Hour				hh
    $M		Minute				mm
    $S		Second				ss
    $J		MilliSecond		mm

    $N		Date				yyyy:mm:dd:
    $Y		Date Year			yyyy
    $O		Date Month			mm
    $D		Date Day			dd

    $F		Func. Name			main, foo
    $A		File path           C:\Project\main.c C:\Project\foo.c
    $K		shortenedFile Name  Project\main.c Project\foo.c
    $I		File Name           main.c foo.c
    $G		Line				1, 42
    $P      thread id           b5bff640

    $L		LogLevel			[TRACE], [DEBUG] … [FATAL]
    $X		Alignment			add space for "INFO" & "WARN"
    $B		Color Begin			from here the color starts
    $E		Color End			from here the color ends
    $C		Text				Formatted Message with variables
    $Z		New Line			Adds a new Line to the log*/
void logger_core_set_formatting(char* format);
void logger_core_use_formatting_backup(void);
void logger_core_set_format_for_specific_log_level(enum log_level level, char* Format);
void logger_core_disable_format_for_specific_log_level(enum log_level level);

void logger_shutdown();
int logger_register_thread_log_under_name(uintptr_t threadID, const char* name);
void logger_set_log_level(enum log_level new_level);
void logger_core_log_msg(const enum log_level level, const char* prefix, const char* funcName, char* fileName, const int Line, const uintptr_t thread_id, const char* message, ...);

// Define witch log levels should be written to log file directly and witch should be buffered
//  0    =>   write all logs directly to log file
//  1    =>   buffer: TRACE
//  2    =>   buffer: TRACE + DEBUG
//  3    =>   buffer: TRACE + DEBUG + INFO
//  4    =>   buffer: TRACE + DEBUG + INFO + WARN
PFF_API void logger_set_buffer_level(int newLevel);
PFF_API void logger_log_msg(const enum log_level level, const char* prefix, const char* funcName, char* fileName, const int Line, const uintptr_t thread_id, const char* message, ...);
PFF_API int logger_is_initalized(void);

PFF_API void logger_set_format(char* format);
PFF_API void logger_use_format_backup(void);
PFF_API void logger_print_separator(uintptr_t threadID);
PFF_API void logger_print_separator_big(uintptr_t threadID);

PFF_API int logger_init(const char* LogFileName, const char* directoryName, char* GeneralLogFormat, uintptr_t threadID, int Use_separate_Files_for_every_Thread);


// ------------------------------------------------------------------------------ LOGGING ------------------------------------------------------------------------------


// Debug breakpoint macro
#if defined(__GNUC__) || defined(__clang__)
#define DEBUG_BREAK      __asm__("int $3")
#elif defined(_MSC_VER)
#define DEBUG_BREAK      __debugbreak()
#else
#define DEBUG_BREAK      // Unsupported platform, do nothing 
#endif




#ifdef PFF_INSIDE_ENGINE

    // ------------------------------------------------------------------------------ CORE LOGGING ------------------------------------------------------------------------------

    #define CORE_LOG_Fatal(message, ...)                    { logger_core_log_msg(Fatal, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
    #define CORE_LOG_Error(message, ...)                    { logger_core_log_msg(Error, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }


    // define conditional log macro for WARNINGS
    #if LOG_LEVEL_ENABLED >= 1
        #define CORE_LOG_Warn(message, ...)                 { logger_core_log_msg(Warn, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
    #else
        #define CORE_LOG_Warn(message, ...)                 {}
    #endif

    // define conditional log macro for INFO
    #if LOG_LEVEL_ENABLED >= 2
        #define CORE_LOG_Info(message, ...)                 { logger_core_log_msg(Info, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
        #define CORE_LOG_INIT_SUBSYS(message, ...)          { logger_core_log_msg(Info, "Initialized Sub-System: ", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
    #else
        #define CORE_LOG_Info(message, ...)                 {}
        #define CORE_LOG_INIT_SUBSYS(message, ...)          {}
    #endif

    // define conditional log macro for DEBUG
    #if LOG_LEVEL_ENABLED >= 3
        #define CORE_LOG_Debug(message, ...)                { logger_core_log_msg(Debug, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
    #else
        #define CORE_LOG_Debug(message, ...)                {}
    #endif

    // define conditional log macro for TRACE
    #if LOG_LEVEL_ENABLED >= 4
        #define CORE_LOG_Trace(message, ...)                { logger_core_log_msg(Trace, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }

        // Logs the start of a function, it would be helpful to has the '$F' in your format
        #define CORE_LOG_FUNC_START(message, ...)           { logger_core_log_msg(Trace, "START ", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }

        // Logs the end of a function, it would be helpful to has the '$F' in your format
        #define CORE_LOG_FUNC_END(message, ...)             { logger_core_log_msg(Trace, "END ", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }

        // Insert a separation line in Log output (-------)
        #define CORE_SEPARATOR()                            { logger_print_separator(CL_THREAD_ID); }

        // Insert a separation line in Log output (=======)
        #define CORE_SEPARATOR_BIG()                        { logger_print_separator_big(CL_THREAD_ID); }
    #else
        #define CORE_LOG_Trace(message, ...) ;
        #define CORE_LOG_FUNC_END(message, ...)             {}
        #define CORE_LOG_FUNC_START(message, ...)           {}
        #define CORE_SEPARATOR()                            {}
        #define CORE_SEPARATOR_BIG()                        {}
    #endif

    // ------------------------------------------------------------------------------ VALIDATION / ASSERTION ------------------------------------------------------------------------------

    #define CORE_ERROR_EXIT(messageSuccess, ...)                                                     CORE_LOG_Fatal(messageSuccess, ##__VA_ARGS__) exit(1)
    #define CORE_ERROR_RETURN(messageSuccess, retVal, ...)                                           CORE_LOG_Error(messageSuccess, ##__VA_ARGS__) return retVal 

    // ----------------------------------------------- VALIDATION -----------------------------------------------

    #define CORE_LOG(Type, message, ...)                CORE_LOG_##Type(message, ##__VA_ARGS__)

    #ifdef VALIDATE_ENABLED
        #define CORE_VALIDATE(expr, abortCommand, messageSuccess, messageFailure, ...)              \
                if (expr) {                                                                         \
                    CORE_LOG_Trace(messageSuccess, ##__VA_ARGS__)                                   \
                } else {                                                                            \
                    CORE_LOG_Error(messageFailure, ##__VA_ARGS__)                                   \
                    abortCommand;                                                                   \
                }
    #else
        #define CORE_VALIDATE(expr, abortCommand, messageSuccess, messageFailure, ...)            {if(!expr){abortCommand}}
    #endif 

    // ----------------------------------------------- ASSERTION -----------------------------------------------

    #ifdef ASSERT_ENABLED
        #define CORE_ASSERT(expr, messageSuccess, messageFailure, ...)                              \
                if (expr) {                                                                         \
                    CORE_LOG_Trace(messageSuccess, ##__VA_ARGS__)                                   \
                } else {                                                                            \
                    CORE_LOG_Fatal(messageFailure, ##__VA_ARGS__)                                   \
                    DEBUG_BREAK;                                                                    \
                }

        #define CORE_ASSERT_PLUS(expr, FailureCommand, messageSuccess, messageFailure, ...)         \
                if (expr) {                                                                         \
                    CORE_LOG_Trace(messageSuccess, ##__VA_ARGS__)                                   \
                } else {                                                                            \
                    FailureCommand;                                                                 \
                    CORE_LOG_Fatal(messageFailure, ##__VA_ARGS__)                                   \
                    DEBUG_BREAK;                                                                    \
                }
    #else
        #define CORE_ASSERT(expr, messageSuccess, messageFailure, ...)                            {expr;}
        #define CORE_ASSERT_PLUS(expr, FailureCommand, messageSuccess, messageFailure, ...)       {if(!expr){FailureCommand}}
    #endif

#else

    // ------------------------------------------------------------------------------ CLIENT LOGGING ------------------------------------------------------------------------------

    #define LOG_Fatal(message, ...)                  { logger_log_msg(Fatal, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
    #define LOG_Error(message, ...)                  { logger_log_msg(Error, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }


    // define conditional log macro for WARNINGS
    #if LOG_LEVEL_ENABLED >= 1
        #define LOG_Warn(message, ...)               { logger_log_msg(Warn, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
    #else
        #define LOG_Warn(message, ...)               {}
    #endif

    // define conditional log macro for INFO
    #if LOG_LEVEL_ENABLED >= 2
        #define LOG_Info(message, ...)               { logger_log_msg(Info, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
    #else
        #define LOG_Info(message, ...)               {}
    #endif

    // define conditional log macro for DEBUG
    #if LOG_LEVEL_ENABLED >= 3
        #define LOG_Debug(message, ...)              { logger_log_msg(Debug, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
    #else
        #define LOG_Debug(message, ...)              {}
    #endif

    // define conditional log macro for TRACE
    #if LOG_LEVEL_ENABLED >= 4
        #define LOG_Trace(message, ...)              { logger_log_msg(Trace, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }

        // Logs the start of a function, it would be helpful to has the '$F' in your format
        #define LOG_FUNC_START(message, ...)         { logger_log_msg(Trace, "START ", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }

        // Logs the end of a function, it would be helpful to has the '$F' in your format
        #define LOG_FUNC_END(message, ...)           { logger_log_msg(Trace, "END ", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }

        // Insert a separation line in Log output (-------)
        #define CL_SEPARATOR()                       { logger_print_separator(CL_THREAD_ID); }

        // Insert a separation line in Log output (=======)
        #define CL_SEPARATOR_BIG()                   { logger_print_separator_big(CL_THREAD_ID); }
    #else
        #define LOG_Trace(message, ...) ;
        #define LOG_FUNC_END(message, ...)           {}
        #define LOG_FUNC_START(message, ...)         {}
        #define CL_SEPARATOR()                       {}
        #define CL_SEPARATOR_BIG()                   {}
    #endif


    #define LOG(Type, message, ...)                  LOG_##Type(message, ##__VA_ARGS__)

    // ------------------------------------------------------------------------------ VALIDATION / ASSERTION ------------------------------------------------------------------------------

    #define ERROR_EXIT(messageSuccess, ...)                                                     LOG_Fatal(messageSuccess, ##__VA_ARGS__) exit(1)
    #define ERROR_RETURN(messageSuccess, retVal, ...)                                           LOG_Error(messageSuccess, ##__VA_ARGS__) return retVal 

    // ----------------------------------------------- VALIDATION -----------------------------------------------

    #ifdef VALIDATE_ENABLED
        #define VALIDATE(expr, abortCommand, messageSuccess, messageFailure, ...)               \
            if (expr) {                                                                         \
                LOG_Trace(messageSuccess, ##__VA_ARGS__)                                        \
            } else {                                                                            \
                LOG_Error(messageFailure, ##__VA_ARGS__)                                        \
                abortCommand;                                                                   \
            }
    #else
        #define VALIDATE(expr, abortCommand, messageSuccess, messageFailure, ...)            {if(!expr){abortCommand}}
    #endif 

    // ----------------------------------------------- ASSERTION -----------------------------------------------

    #ifdef ASSERT_ENABLED
        #define ASSERT(expr, messageSuccess, messageFailure, ...)                               \
            if (expr) {                                                                         \
                LOG_Trace(messageSuccess, ##__VA_ARGS__)                                        \
            } else {                                                                            \
                LOG_Fatal(messageFailure, ##__VA_ARGS__)                                        \
                DEBUG_BREAK;                                                                    \
            }
        
        #define ASSERT_PLUS(expr, FailureCommand, messageSuccess, messageFailure, ...)          \
            if (expr) {                                                                         \
                LOG_Trace(messageSuccess, ##__VA_ARGS__)                                        \
            } else {                                                                            \
                FailureCommand;                                                                 \
                LOG_Fatal(messageFailure, ##__VA_ARGS__)                                        \
                __debugbreak();                                                                 \
                DEBUG_BREAK;                                                                    \
            }
    #else
        #define ASSERT(expr, messageSuccess, messageFailure, ...)                            {expr;}
        #define ASSERT_PLUS(expr, FailureCommand, messageSuccess, messageFailure, ...)       {if(!expr){FailureCommand}}
    #endif 

#endif























// ------------------------------------------------------------------------------ MEASURE EXECUTION TIME ------------------------------------------------------------------------------

// MOVE to pff_time
void Calc_Func_Duration_Start(struct CL_Time_Info* StartTime);
void Calc_Func_Duration(const struct CL_Time_Info* StartTime);


// Remembers the exact time at witch this macro was called
// CAUTION! only call once in a given scope
#define CL_FUNC_DURATION_START()                struct CL_Time_Info Log_Duration_Calc_Struct_Start;       \
                                                Calc_Func_Duration_Start(&Log_Duration_Calc_Struct_Start);

// Calculates the time difference between calling [CL_FUNC_DURATION_START] and this Macro
#define CL_FUNC_DURATION()                      Calc_Func_Duration(&Log_Duration_Calc_Struct_Start);
