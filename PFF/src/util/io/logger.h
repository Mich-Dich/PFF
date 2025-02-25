#pragma once
#pragma warning(disable: 4251)

#include "util/core_config.h"

#undef ERROR

#ifndef DEBUG_BREAK
	#define DEBUG_BREAK() __debugbreak()
#endif // !DEBUG_BREAK

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
        Fatal
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
    // @param $A file name               /home/workspace/test_cpp/src/main.cpp  /home/workspace/test_cpp/src/project.cpp
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

    // all messages with a lower severity than the provided argumant will be buffered
    // Trace => buffer[]
    // Debug => buffer[Trace]
    // Info  => buffer[Trace + Debug]
    // Warn  => buffer[Trace + Debug + Info]
    // Error => buffer[Trace + Debug + Info + Warn]     (Error and Fatal will nover be buffered)
    // Fatal => buffer[Trace + Debug + Info + Warn]     (Error and Fatal will nover be buffered)
    void set_buffer_threshhold(const severity new_threshhold);

    // set the size of the buffer.
    // @note for messages that are not directly logged
    void set_buffer_size(const size_t new_size);

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
    
    // THIS SHOULD NEVER BE DIRECTLY CALLED
    // @note empty log messages will be ignored
    void log_msg(const severity msg_sev, const char* file_name, const char* function_name, const int line, std::thread::id thread_id, std::string&& message);


	class logged_exception : public std::exception {
		public: 
			explicit logged_exception(const char* file, const char* function , const int line, std::thread::id thread_id,  std::string&& message)
				: m_msg(message) { PFF::logger::log_msg(PFF::logger::severity::Error, file, function, line, thread_id, std::move(message)); }
		
			virtual const char* what() const noexcept override { return m_msg.c_str(); }
		
		private:
			std::string m_msg;
	};
}

// This enables the diffrent log levels (FATAL + ERROR are alwasy on)
//  0 = FATAL + ERROR
//  1 = FATAL + ERROR + WARN
//  2 = FATAL + ERROR + WARN + INFO
//  3 = FATAL + ERROR + WARN + INFO + DEBUG
//  4 = FATAL + ERROR + WARN + INFO + DEBUG + TRACE
#define LOG_LEVEL_ENABLED           			4


//  ===================================================================================  Logger calls  ===================================================================================


#define LOGGED_EXCEPTION(message)   { std::ostringstream oss{}; oss << "LOGGER EXCEPTION: " << message; throw logger::logged_exception(__FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), std::move(oss.str())); }

#define LOG_Fatal(message)          { std::ostringstream oss{}; oss << message; PFF::logger::log_msg(PFF::logger::severity::Fatal, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), std::move(oss.str())); }
#define LOG_Error(message)          { std::ostringstream oss{}; oss << message; PFF::logger::log_msg(PFF::logger::severity::Error, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), std::move(oss.str())); }

#if LOG_LEVEL_ENABLED > 0
    #define LOG_Warn(message)       { std::ostringstream oss{}; oss << message; PFF::logger::log_msg(PFF::logger::severity::Warn, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), std::move(oss.str())); }
#else
    #define LOG_Warn(message)       { }
#endif

#if LOG_LEVEL_ENABLED > 1
    #define LOG_Info(message)       { std::ostringstream oss{}; oss << message; PFF::logger::log_msg(PFF::logger::severity::Info, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), std::move(oss.str())); }
#else
    #define LOG_Info(message)       { }
#endif

#if LOG_LEVEL_ENABLED > 2
    #define LOG_Debug(message)      { std::ostringstream oss{}; oss << message; PFF::logger::log_msg(PFF::logger::severity::Debug, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), std::move(oss.str())); }
#else
    #define LOG_Debug(message)      { }
#endif

#if LOG_LEVEL_ENABLED > 3
    #define LOG_Trace(message)      { std::ostringstream oss{}; oss << message; PFF::logger::log_msg(PFF::logger::severity::Trace, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), std::move(oss.str())); }
#else
    #define LOG_Trace(message)      { }
#endif


#define LOG(severity, message)      LOG_##severity(message)


// ---------------------------------------------------------------------------  Assertion & Validation  ---------------------------------------------------------------------------

#if ENABLE_LOGGING_FOR_ASSERTS
    #define ASSERT(expr, message_success, message_failure)                              \
        if (expr)                                                                       \
            LOG(Trace, message_success)                                                 \
        else {                                                                          \
            LOG(Fatal, message_failure)                                                 \
            LOGGED_EXCEPTION(message_failure);                                          \
        }

    #define ASSERT_S(expr)                                                              \
        if (!(expr)) {                                                                  \
            LOG(Fatal, #expr)                                                           \
            LOGGED_EXCEPTION(#expr);                                                    \
        }
#else
    #define ASSERT(expr, message_success, message_failure)                              if (!(expr)) { LOGGED_EXCEPTION(#expr); }
    #define ASSERT_S(expr)                                                              if (!(expr)) { LOGGED_EXCEPTION(#expr); }
#endif

#if ENABLE_LOGGING_FOR_VALIDATION
    #define VALIDATE(expr, command, message_success, message_failure)                   \
        if (expr)                                                                       \
            LOG(Trace, message_success)                                                 \
        else {                                                                          \
            LOG(Error, message_failure)                                                 \
            command;                                                                    \
        }

    #define VALIDATE_S(expr, command)                                                   \
        if (!(expr)) {                                                                  \
            LOG(Error, #expr)                                                           \
            command;                                                                    \
        }
#else
    #define VALIDATE(expr, command, message_success, message_failure)                   if (!(expr)) { command; }
    #define VALIDATE_S(expr, command)                                                   if (!(expr)) { command; }
#endif


#define LOG_INIT()																		LOG(Trace, "init");
#define LOG_SHUTDOWN()																	LOG(Trace, "shutdown");
