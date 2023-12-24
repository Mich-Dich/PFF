#pragma once

#include <errno.h>
#include <stdint.h>


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

enum log_level {
    Fatal = 0,
    Error = 1,
    Warn = 2,
    Info = 3,
    Debug = 4,
    Trace = 5,
    LL_MAX_NUM = 6
};

typedef struct {
    int TI_year;
    int TI_month;
    int TI_day;
    int TI_hour;
    int TI_minute;
    int TI_sec;
    int TI_nSec;
} Time_Info;

#define CL_TRUE 1
#define CL_FALSE 0


// ------------------------------------------------------------------------------ Main Functions ------------------------------------------------------------------------------

/*  Formatting the LogMessages can be customized with the following tags
    to format all following Log Messages use: set_Formatting(char* format);
    e.g. set_Formatting("$B[$T] $L [$F]  $C$E")  or set_Formatting("$BTime:[$M $S] $L $E ==> $C")
    
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
    $A		File Name			C:\Project\main.c C:\Project\foo.c
    $I		shortened File Name	main.c foo.c
    $G		Line				1, 42
    $P      thread id           b5bff640

    $L		LogLevel			[TRACE], [DEBUG] … [FATAL]
    $X		Alignment			add space for "INFO" & "WARN"
    $B		Color Begin			from here the color starts
    $E		Color End			from here the color ends
    $C		Text				Formatted Message with variables
    $Z		New Line			Adds a new Line to the log*/
void set_Formatting(char* format);
void use_Formatting_Backup();
void Set_Format_For_Specific_Log_Level(enum log_level level, char* Format);
void Disable_Format_For_Specific_Log_Level(enum log_level level);

int log_init(const char* LogFileName, const char *directoryName, char* GeneralLogFormat, uintptr_t threadID, int Use_separate_Files_for_every_Thread);
void log_output(const enum log_level level, const char* prefix, const char* funcName, char* fileName, const int Line, const uintptr_t thread_id, const char* message, ...);
void print_Separator(uintptr_t threadID);
void print_Separator_Big(uintptr_t threadID);
int register_thread_log_under_Name(uintptr_t threadID, const char* name);
void log_shutdown();
void set_log_level(enum log_level new_level);
void Calc_Func_Duration_Start(Time_Info* StartTime);
void Calc_Func_Duration(const Time_Info* StartTime);

// Define witch log levels should be written to log file directly and witch should be buffered
//  0    =>   write all logs directly to log file
//  1    =>   buffer: TRACE
//  2    =>   buffer: TRACE + DEBUG
//  3    =>   buffer: TRACE + DEBUG + INFO
//  4    =>   buffer: TRACE + DEBUG + INFO + WARN
void set_buffer_Level(int newLevel);


// ------------------------------------------------------------------------------ Helper Functions ------------------------------------------------------------------------------

// checks pointers and returns " NULL" or "valid"
static inline const char* ptr_To_String(void* pointer) { return (pointer == NULL) ? " NULL" : "valid"; }

// ------------------------------------------------------------------------------ LOGGING ------------------------------------------------------------------------------


// Debug breakpoint macro
#if defined(__GNUC__) || defined(__clang__)
    #define DEBUG_BREAK      __asm__("int $3")
#elif defined(_MSC_VER)
    #define DEBUG_BREAK      __debugbreak()
#else
    #define DEBUG_BREAK      /* Unsupported platform, do nothing */
#endif

// ------------------------------------------------------------------------------ LOGGING ------------------------------------------------------------------------------

#define CL_LOG_Fatal(message, ...)                  { log_output(Fatal, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
#define CL_LOG_Error(message, ...)                  { log_output(Error, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }


// define conditional log macro for WARNINGS
#if LOG_LEVEL_ENABLED >= 1
    #define CL_LOG_Warn(message, ...)               { log_output(Warn, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
#else
    // Disabled by LogLevel
    #define CL_LOG_Warn(message, ...)               {} while(0);
#endif

// define conditional log macro for INFO
#if LOG_LEVEL_ENABLED >= 2
    #define CL_LOG_Info(message, ...)               { log_output(Info, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
                 
#else
    // Disabled by LogLevel
    #define CL_LOG_Info(message, ...)               {} while(0);
#endif

// define conditional log macro for DEBUG
#if LOG_LEVEL_ENABLED >= 3
    #define CL_LOG_Debug(message, ...)              { log_output(Debug, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }

    // Logs the end of a function, it would be helpful to has the '$F' in your format
    #define CL_LOG_FUNC_END(message, ...)           { log_output(Debug, "END ", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }

    // Logs the start of a function, it would be helpful to has the '$F' in your format
    #define CL_LOG_FUNC_START(message, ...)         { log_output(Debug, "START ", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
#else
    // Disabled by LogLevel
    #define CL_LOG_Debug(message, ...)              {} while(0);
    // Disabled by LogLevel
    #define CL_LOG_FUNC_END(message, ...)           {} while(0);
    // Disabled by LogLevel
    #define CL_LOG_FUNC_START(message, ...)         {} while(0);
#endif

// define conditional log macro for TRACE
#if LOG_LEVEL_ENABLED >= 4
    #define CL_LOG_Trace(message, ...)              { log_output(Trace, "", __func__, __FILE__, __LINE__, CL_THREAD_ID, message, ##__VA_ARGS__); }
    // Insert a separation line in Log output (-------)
    #define CL_SEPARATOR()                          { print_Separator(CL_THREAD_ID); }
    // Insert a separation line in Log output (=======)
    #define CL_SEPARATOR_BIG()                      { print_Separator_Big(CL_THREAD_ID); }
#else
    // Disabled by LogLevel
    #define CL_LOG_Trace(message, ...) ;
    // Disabled by LogLevel
    #define CL_SEPARATOR()                          {} while(0);
    #define CL_SEPARATOR_BIG()                      {} while(0);
#endif


#define CL_LOG(Type, message, ...)                  CL_LOG_##Type(message, ##__VA_ARGS__)

// ------------------------------------------------------------------------------ VALIDATION / ASSERTION ------------------------------------------------------------------------------

#define ERROR_EXIT(messageSuccess, ...)                                                     CL_LOG_Fatal(messageSuccess, ##__VA_ARGS__) exit(1)
#define ERROR_RETURN(messageSuccess, retVal, ...)                                           CL_LOG_Error(messageSuccess, ##__VA_ARGS__) return retVal 

// ----------------------------------------------- VALIDATION -----------------------------------------------

#ifdef VALIDATE_ENABLED
    #define CL_VALIDATE(expr, abortCommand, messageSuccess, messageFailure, ...)            \
        if (expr) {                                                                         \
            CL_LOG_Trace(messageSuccess, ##__VA_ARGS__)                                     \
        } else {                                                                            \
            CL_LOG_Error(messageFailure, ##__VA_ARGS__)                                     \
            abortCommand;                                                                   \
        }
#else
    #define CL_VALIDATE(expr, abortCommand, messageSuccess, messageFailure, ...)            {expr;}
#endif 

// ----------------------------------------------- ASSERTION -----------------------------------------------

#ifdef ASSERT_ENABLED
    #define CL_ASSERT(expr, messageSuccess, messageFailure, ...)                            \
        if (expr) {                                                                         \
            CL_LOG_Trace(messageSuccess, ##__VA_ARGS__)                                     \
        } else {                                                                            \
            CL_LOG_Fatal(messageFailure, ##__VA_ARGS__)                                     \
            DEBUG_BREAK;                                                                    \
        }
        
    #define CL_ASSERT_PLUS(expr, FailureCommand, messageSuccess, messageFailure, ...)       \
        if (expr) {                                                                         \
            CL_LOG_Trace(messageSuccess, ##__VA_ARGS__)                                     \
        } else {                                                                            \
            FailureCommand;                                                                 \
            CL_LOG_Fatal(messageFailure, ##__VA_ARGS__)                                     \
            __debugbreak();                                                         \
            DEBUG_BREAK;                                                                    \
        }
#else
    #define CL_ASSERT(expr, messageSuccess, messageFailure, ...)                            {expr;}
    #define CL_ASSERT_PLUS(expr, FailureCommand, messageSuccess, messageFailure, ...)       {expr;}
#endif 

// ------------------------------------------------------------------------------ MEASURE EXECUTION TIME ------------------------------------------------------------------------------

// Remembers the exact time at witch this macro was called
// CAUTION! only call once in a given scope
#define CL_FUNC_DURATION_START()                Time_Info Log_Duration_Calc_Struct_Start;       \
                                                Calc_Func_Duration_Start(&Log_Duration_Calc_Struct_Start);

// Calculates the time difference between calling [CL_FUNC_DURATION_START] and this Macro
#define CL_FUNC_DURATION()                      Calc_Func_Duration(&Log_Duration_Calc_Struct_Start);
