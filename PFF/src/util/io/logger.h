#pragma once

#include "util/core_config.h"

namespace PFF::logger {

    // Define the severity levels for logging
    // @note severity Enum representing the levels of logging severity
    // @note Trace The lowest level, used for tracing program execution
    // @note Debug Used for detailed debug information
    // @note Info Informational messages that highlight progress
    // @note Warn Messages for potentially harmful situations
    // @note Error Error events that might still allow the application to continue
    // @note Fatal Severe error events that lead to application shutdown
    enum class severity : u8 {
        Trace = 0,
        Debug,
        Info,
        Warn,
        Error,
        Fatal,
    };

    // Structure to represent the format of a log message
    // @struct message_format Encapsulates details for a log message
    // @param msg_sev The severity level of the message
    // @param file_name The name of the file where the log message originated
    // @param function_name The function name where the log message was generated
    // @param line The line number in the source file of the log message
    // @param message The actual log message content
    struct message_format {

        message_format(const logger::severity msg_sev, const char* file_name, const char* function_name, const int line, std::thread::id thread_id, const std::string& message) 
            : msg_sev(msg_sev), file_name(file_name), function_name(function_name), line(line), thread_id(thread_id), message(message) {};

        const logger::severity  msg_sev;
        const char*             file_name;
        const char*             function_name;
        const int               line;
        const std::thread::id   thread_id;
        const std::string       message;
    };

    // Initalize the logging system
    // @param format The iital log message foemat
    // @param log_to_console should the log message be written to std::cout?
    // @param log_dir the directory that will contain all log files
    // @ main_log_file_name name of the central log_file (the thread that runs logger::init())
    // @param use_append_mode Should the system write over the existing log file or append to it
    bool init(const std::string& format, const bool log_to_console = false, const std::filesystem::path log_dir = "./logs", const std::string& main_log_file_name = "general.log", const bool use_append_mode = false);

    // shutdown the logging system
    void shutdown();

    // The format of log-messages can be custimized with the following tags
    // @note to format all following log-messages use: set_format()
    // @note e.g. set_format("$B[$T] $L [$F] $C$E")
    //
    // @param $T time                    hh:mm:ss
    // @param $H hour                    hh
    // @param $M minute                  mm
    // @param $S secunde                 ss
    // @param $J millisecunde            jjj
    //      
    // @param $N data                    yyyy:mm:dd
    // @param $Y data year               yyyy
    // @param $O data month              mm
    // @param $D data day                dd
    //
    // @param $M thread                  Thread_id: 137575225550656 or a lable if provided
    // @param $F function name           application::main, math::foo
    // @param $P only function name      main, foo
    // @param $A file name               main.cpp  project.cpp
    // @param $I only file name          main.cpp
    // @param $G line                    1, 42
    //
    // @param $L log-level               add used log severity: [TRACE], [DEBUG] ... [FATAL]
    // @param $X alignment               adds space for "INFO" & "WARN"
    // @param $B color begin             from here the color begins
    // @param $E color end               from here the color will be reset
    // @param $C text                    the message the user wants to print
    // @param $Z new line                add a new line in the message format
    void set_format(const std::string& new_format);

    // Restore the previous log-message format
    // @note This function swaps the current log-message format with the previously stored backup.
    // It's useful for revertinng to the previous format after temporary changes
    void use_previous_format();

    // get the currently used log-message format
    const std::string get_format();

    // Registers a label for a specific thread, allowing for easier identification in logs.
    // If a label is already registered for the given thread ID, it will be overridden with the new label.
    // @param thread_label The label to be associated with the thread.
    // @param thread_id The ID of the thread for which the label is being registered. 
    //                  Defaults to the ID of the calling thread if not provided.
    void register_label_for_thread(const std::string& thread_lable, std::thread::id thread_id = std::this_thread::get_id());
    
    // Unregisters the label for a specific thread, removing its association from the logger.
    // If no label is registered for the given thread ID, a message will be logged indicating that the operation was ignored.
    // @param thread_id The ID of the thread for which the label is being unregistered. 
    //                  Defaults to the ID of the calling thread if not provided.
    void unregister_label_for_thread(std::thread::id thread_id = std::this_thread::get_id());

    // // THIS SHOULD NEVER BE DIRECTLY CALLED
    // // @note empty log messages will be ignored
    void log_msg(const severity msg_sev , const char* file_name, const char* function_name, const int line, const std::thread::id thread_id, const std::string& message);
}



// Exception to represent a debug break
// @class debug_break_exception Exception type for debug breaks
// @param message The error message associated with the debug break
class debug_break_exception : public std::exception {
public:
    explicit debug_break_exception(const std::string& message)
        : m_msg(message) {
			
			try {
				PFF::logger::log_msg(PFF::logger::severity::Fatal, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), m_msg);
			} catch(...) {
				std::cerr << m_msg << std::endl;
			}
		}

    virtual const char* what() const noexcept override { return m_msg.c_str(); }

private:
    std::string m_msg;
};

// Macro to trigger a debug break with detailed context
// @param message The custom message to include in the debug break exception
// @note DEBUG_BREAK Triggers a debug break exception with a formatted message
// @note Constructs a detailed message containing the file name, function name, and line number
#define DEBUG_BREAK(message)                { std::ostringstream oss; oss << "DEBUG BREAK [file: " << __FILE__ << ", function: " << __FUNCTION__ << ", line: " << __LINE__ << "] => "<< message; throw debug_break_exception(oss.str()); }



// This enables the verious levels of the logging function (FATAL & ERROR are always on)
//  0    =>   FATAL + ERROR
//  1    =>   FATAL + ERROR + WARN
//  2    =>   FATAL + ERROR + WARN + INFO
//  3    =>   FATAL + ERROR + WARN + INFO + DEBUG
//  4    =>   FATAL + ERROR + WARN + INFO + DEBUG + TRACE
#define LOG_LEVEL_ENABLED 4


//  =================================================================================== Logger  ===================================================================================

// I use std::ostringstream here instead of lamdas because the logger runs async and I want to capture the values in pointers/refs in the moment the macro is called
// This costs some performance but is more useful when debugging an application

// always enabled
#define LOG_Fatal(message)                  { std::ostringstream oss; oss << message; PFF::logger::log_msg(PFF::logger::severity::Fatal, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), oss.str()); }
#define LOG_Error(message)                  { std::ostringstream oss; oss << message; PFF::logger::log_msg(PFF::logger::severity::Error, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), oss.str()); }

#if LOG_LEVEL_ENABLED > 0
    #define LOG_Warn(message)               { std::ostringstream oss; oss << message; PFF::logger::log_msg(PFF::logger::severity::Warn, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), oss.str()); }
#else
    #define LOG_Warn(message)               { }
#endif

#if LOG_LEVEL_ENABLED > 1
    #define LOG_Info(message)               { std::ostringstream oss; oss << message; PFF::logger::log_msg(PFF::logger::severity::Info, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), oss.str()); }
#else
    #define LOG_Info(message)               { }
#endif

#if LOG_LEVEL_ENABLED > 2
    #define LOG_Debug(message)              { std::ostringstream oss; oss << message; PFF::logger::log_msg(PFF::logger::severity::Debug, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), oss.str()); }
#else
    #define LOG_Debug(message)              { }
#endif

#if LOG_LEVEL_ENABLED > 3
    #define LOG_Trace(message)              { std::ostringstream oss; oss << message; PFF::logger::log_msg(PFF::logger::severity::Trace, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), oss.str()); }
    #define LOG_SEPERATOR                   { PFF::logger::log_msg(PFF::logger::severity::Trace, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), "-------------------------------------------------------------"); }
#else
    #define LOG_Trace(message)              { }
    #define LOG_SEPERATOR                   { }
#endif

// General logging macro for all severity levels
// @note LOG Routes log messages to the appropriate severity level
// @param severity The severity level of the log (e.g., Trace, Debug, Info, Warn, Error, Fatal)
// @param message The message to be logged
// @note This macro resolves to one of the specific logging macros (e.g., LOG_Trace, LOG_Debug) based on the provided severity
// @note LOG(Info, "This is an informational message");
// @note LOG(Error, "An error occurred while processing the request");
#define LOG(severity, message)              LOG_##severity(message)

#define LOG_INIT()							LOG_Trace( "init");
#define LOG_SHUTDOWN()						LOG_Trace( "shutdown");

// ---------------------------------------------------------------------------  Assertion & Validation  ---------------------------------------------------------------------------

#if ENABLED_LOGGING_OF_ASSERTS
    #define ASSERT(expr, successMsg, failureMsg)							\
                    if (expr) 												\
                        LOG(Trace, successMsg)							    \
                    else {												    \
                        LOG(Fatal, failureMsg)						        \
                        DEBUG_BREAK(failureMsg);							\
                    }

    #define ASSERT_S(expr)													\
                    if (!(expr)) {									        \
                        LOG(Fatal, #expr)						            \
                        DEBUG_BREAK(#expr);     					        \
                    }
#else
    #define ASSERT(expr, successMsg, failureMsg)							if (!(expr)) { DEBUG_BREAK(failureMsg); }
    #define ASSERT_S(expr)													if (!(expr)) { DEBUG_BREAK(#expr); }
#endif // ENABLED_LOGGING_OF_ASSERT


#if ENABLE_LOGGING_OF_VALIDATION
    #define VALIDATE(expr, command, successMsg, failureMsg)			        \
                    if (expr) 												\
                        LOG(Trace, successMsg)							    \
                    else {													\
                        LOG(Warn, failureMsg)							    \
                        command;										    \
                    }

    #define VALIDATE_S(expr, command)									    \
                    if (!(expr)) {											\
                        LOG(Warn, "Validation Failed: " << #expr)		    \
                        command;										    \
                    }
#else
    #define VALIDATE(expr, ReturnCommand, successMsg, failureMsg)			if (!(expr)) { ReturnCommand; }
    #define VALIDATE_S(expr, ReturnCommand)						            if (!(expr)) { ReturnCommand; }

#endif // ENABLE_LOGGING_OF_VALIDATION
