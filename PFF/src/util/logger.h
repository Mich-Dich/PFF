#pragma once
#pragma warning(disable: 4251)

#include "util/core_config.h"

#undef ERROR

#ifndef DEBUG_BREAK
	#define DEBUG_BREAK() __debugbreak()
#endif // !DEBUG_BREAK

namespace PFF {

	class exception : public std::runtime_error {
	public:
		using std::runtime_error::runtime_error;
	};

	namespace logger {

		enum log_msg_severity {
			Trace = 0,
			Debug = 1,
			Info = 2,
			Warn = 3,
			Error = 4,
			Fatal = 5,
			NUM_SEVERITIES = 6
		};

		// Initializes the logging system.
		// @param [Format] The initial log format.
		// @return [bool] Returns true if the initialization is successful, false otherwise.
		PFF_API bool init(const std::string& format);

		// Formating the LogMessages can be customised with the following tags
		// to format all following Log Messages use: set_Formating(char* format);
		// e.g. set_Formating("$B[$T] $L [$F]  $C$E")  or set_Formating("$BTime:[$M $S] $L $E ==> $C")
		// 
		// @param $T		Time				hh:mm:ss
		// @param $H		Hour				hh
		// @param $M		Minute				mm
		// @param $S		Secunde				ss
		// @param $J		MilliSecunde			mm
		// @param 
		// @param $N		Date				yyyy:mm:dd:
		// @param $Y		Date Year			yyyy
		// @param $O		Date Month			mm
		// @param $D		Date Day			dd
		// @param 
		// @param $F		Func. Name			application::main, math::foo
		// @param $P		Func. Name			main, foo
		// @param $A		File Name			C:/Porject/main.c C:/Porject/foo.c
		// @param $K		shortend File Name		Porject/main.c Porject/foo.c
		// @param $I		only File Name			main.c foo.c
		// @param $G		Line				1, 42
		// @param 
		// @param $L		LogLevel				[TRACE], [DEBUG] … [FATAL]
		// @param $X		Alienment			add space for "INFO" & "WARN"
		// @param $B		Color Begin			from here the color starts
		// @param $E		Color End			from here the color ends
		// @param $C		Text				Formated Message with variables
		// @param $Z		New Line			Adds a new Line to the log
		PFF_API void set_format(const std::string& format);

		//Restores the previous log message format
		//	@brief This function swaps the current log message format with the previously stored backup.
		//	It is useful for reverting to a previous format after temporary changes.
		PFF_API void use_format_backup();

		// Define witch log levels should be written to log file directly and witch should be buffered
		// @param 0    =>   write all logs directly to log file
		// @param 1    =>   buffer: TRACE
		// @param 2    =>   buffer: TRACE + DEBUG
		// @param 3    =>   buffer: TRACE + DEBUG + INFO
		// @param 4    =>   buffer: TRACE + DEBUG + INFO + WARN
		PFF_API void set_buffer_Level(int newLevel);


		void log_msg(log_msg_severity level, const char* fileName, const char* funcName, int line, const char* message);


		class PFF_API log_message : public std::ostringstream {

		public:
			log_message(log_msg_severity severity, const char* fileName, const char* funcName, int line);
			~log_message();

		private:
			log_msg_severity m_Severity;
			const char* m_FileName;
			const char* m_FuncName;
			int m_Line;

		};
	}
}

// This enables the verious levels of the logging function (FATAL & ERROR are always on)
//  0    =>   FATAL + ERROR
//  1    =>   FATAL + ERROR + WARN
//  2    =>   FATAL + ERROR + WARN + INFO
//  3    =>   FATAL + ERROR + WARN + INFO + DEBUG
//  4    =>   FATAL + ERROR + WARN + INFO + DEBUG + TRACE
#define LOG_LEVEL_ENABLED 4
#define CORE_LOG_LEVEL_ENABLED 4

//  ===================================================================================  Core Logger  ===================================================================================
#if defined(PFF_INSIDE_ENGINE) || defined(PFF_INSIDE_EDITOR)

	#define CORE_LOG_Fatal(message)				{ PFF::logger::log_message(PFF::logger::log_msg_severity::Fatal,__FILE__,__FUNCTION__,__LINE__).flush() << message; }
	#define CORE_LOG_Error(message)				{ PFF::logger::log_message(PFF::logger::log_msg_severity::Error,__FILE__,__FUNCTION__,__LINE__).flush() << message; }

	#if CORE_LOG_LEVEL_ENABLED >= 1
		#define CORE_LOG_Warn(message)				{ PFF::logger::log_message(PFF::logger::log_msg_severity::Warn,__FILE__,__FUNCTION__,__LINE__).flush() << message; }
	#else
		#define CORE_LOG_Warn(message)				{;}
	#endif

	#if CORE_LOG_LEVEL_ENABLED >= 2
		#define CORE_LOG_Info(message)				{ PFF::logger::log_message(PFF::logger::log_msg_severity::Info,__FILE__,__FUNCTION__,__LINE__).flush() << message; }
	#else
		#define CORE_LOG_Info(message)				{;}
	#endif

	#if CORE_LOG_LEVEL_ENABLED >= 3
		#define CORE_LOG_Debug(message)				{ PFF::logger::log_message(PFF::logger::log_msg_severity::Debug,__FILE__,__FUNCTION__,__LINE__).flush() << message; }
	#else
		#define CORE_LOG_Debug(message)				{;}
	#endif

	#if CORE_LOG_LEVEL_ENABLED >= 4
		#define CORE_LOG_Trace(message)				{ PFF::logger::log_message(PFF::logger::log_msg_severity::Trace,__FILE__,__FUNCTION__,__LINE__).flush() << message; }
		#define CORE_LOG_SEPERATOR					PFF::logger::set_format("$C$Z");									\
														CORE_LOG_Trace(PFF::logger::SeperatorStringSmall)				\
														PFF::logger::use_format_backup();

		#define CORE_LOG_SEPERATOR_BIG				PFF::logger::set_format("$C$Z");									\
														CORE_LOG_Trace(PFF::logger::SeperatorStringBig)				\
														PFF::logger::use_format_backup();
	#else
		#define CORE_LOG_Trace(message, ...)		{;}
		#define CORE_LOG_SEPERATOR					{;}
		#define CORE_LOG_SEPERATOR_BIG				{;}
	#endif

	#define CORE_LOG(severity, message)			CORE_LOG_##severity(message)

	#define CORE_LOG_INIT()						CORE_LOG(Trace, "init");
	#define CORE_LOG_SHUTDOWN()					CORE_LOG(Trace, "shutdown");

		// ---------------------------------------------------------------------------  Assertion & Validation  ---------------------------------------------------------------------------

	#if ENABLED_LOGGING_OF_ASSERTS
		#define CORE_ASSERT(expr, successMsg, failureMsg)								\
						if (expr) {														\
							CORE_LOG(Trace, successMsg);								\
						} else {														\
							CORE_LOG(Fatal, failureMsg);								\
							DEBUG_BREAK();												\
						}

		#define CORE_ASSERT_S(expr)														\
						if (!(expr)) {													\
							CORE_LOG(Fatal, #expr);										\
							DEBUG_BREAK();												\
						}
	#else
		#define CORE_ASSERT(expr, successMsg, failureMsg)								if (!(expr)) { DEBUG_BREAK(); }
		#define CORE_ASSERT_S(expr)														if (!(expr)) { DEBUG_BREAK(); }
	#endif // ENABLED_LOGGING_OF_ASSERTS

	#if ENABLE_LOGGING_OF_VALIDATION
		#define CORE_VALIDATE(expr, ReturnCommand, successMsg, failureMsg)				\
						if (expr) {														\
							CORE_LOG(Trace, successMsg);								\
						} else {														\
							CORE_LOG(Warn, failureMsg);									\
							ReturnCommand;												\
						}

		#define CORE_VALIDATE_S(expr, ReturnCommand)									\
						if (!expr) {													\
							CORE_LOG(Warn, #expr);										\
							ReturnCommand;												\
						}
	#else
		#define CORE_VALIDATE(expr, ReturnCommand, successMsg, failureMsg)				if (!(expr)) { ReturnCommand; }
		#define CORE_VALIDATE_S(expr, ReturnCommand)									if (!(expr)) { ReturnCommand; }

	#endif // ENABLE_LOGGING_OF_VALIDATION

	#define STOP(msg) 							CORE_LOG(Fatal, msg);	DEBUG_BREAK();

#endif //PFF_INSIDE_ENGINE

//  ===================================================================================  Client Logger  ===================================================================================

#define LOG_Fatal(message)						{ PFF::logger::log_message(PFF::logger::log_msg_severity::Fatal,__FILE__,__FUNCTION__,__LINE__) << message; }
#define LOG_Error(message)						{ PFF::logger::log_message(PFF::logger::log_msg_severity::Error,__FILE__,__FUNCTION__,__LINE__) << message; }

#if LOG_LEVEL_ENABLED >= 1
	#define LOG_Warn(message, ...)				{ PFF::logger::log_message(PFF::logger::log_msg_severity::Warn,__FILE__,__FUNCTION__,__LINE__) << message; }
#else
	#define LOG_Warn(message, ...)				{;}
#endif

#if LOG_LEVEL_ENABLED >= 2
	#define LOG_Info(message, ...)				{ PFF::logger::log_message(PFF::logger::log_msg_severity::Info,__FILE__,__FUNCTION__,__LINE__) << message; }
#else
	#define LOG_Info(message, ...)				{;}
#endif

#if LOG_LEVEL_ENABLED >= 3
	#define LOG_Debug(message, ...)				{ PFF::logger::log_message(PFF::logger::log_msg_severity::Debug,__FILE__,__FUNCTION__,__LINE__) << message; }
#else
	#define LOG_Debug(message, ...)				{;}
#endif

#if LOG_LEVEL_ENABLED >= 4
	#define LOG_Trace(message, ...)				{ PFF::logger::log_message(PFF::logger::log_msg_severity::Trace,__FILE__,__FUNCTION__,__LINE__) << message; }
#else
	#define LOG_Trace(message, ...)				{;}
#endif

#define LOG(severity, message)					LOG_##severity(message)


#if ENABLED_LOGGING_OF_CLIENT_ASSERTS
	#define ASSERT(expr, successMsg, failureMsg)										\
						if (expr) {														\
							LOG(Trace, successMsg);										\
						} else {														\
							LOG(Fatal, failureMsg);										\
							DEBUG_BREAK();												\
						}

	#define ASSERT_S(expr)																\
						if (!(expr)) {													\
							LOG(Fatal, #expr);											\
							DEBUG_BREAK();												\
						}
#else
	#define ASSERT(expr, successMsg, failureMsg)										if (!(expr)) { DEBUG_BREAK(); }
	#define ASSERT_S(expr)																if (!(expr)) { DEBUG_BREAK(); }
#endif // ENABLED_LOGGING_OF_CLIENT_ASSERTS

#if ENABLE_LOGGING_OF_CLIENT_VALIDATION
	#define VALIDATE(expr, ReturnCommand, successMsg, failureMsg)						\
						if (expr) {														\
							LOG(Trace, successMsg);										\
						} else {														\
							LOG(Warn, failureMsg);										\
							ReturnCommand;												\
						}

	#define VALIDATE_S(expr, ReturnCommand)												\
						if (!expr) {													\
							LOG(Warn, #expr);											\
							ReturnCommand;												\
						}
#else
	#define VALIDATE(expr, ReturnCommand, successMsg, failureMsg)						if (!(expr)) { ReturnCommand; }
	#define VALIDATE_S(expr, ReturnCommand)												if (!(expr)) { ReturnCommand; }
#endif // ENABLE_LOGGING_OF_CLIENT_VALIDATION


#define LOG_INIT()							LOG(Trace, "init");
#define LOG_SHUTDOWN()						LOG(Trace, "shutdown");
