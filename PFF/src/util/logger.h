#pragma once
#pragma warning(disable: 4251)

//#include "glpch.h"
#include <iostream>
#include <string>
#include <sstream>

#include "util/util.h"

#undef ERROR

#ifndef DEBUG_BREAK
	#define DEBUG_BREAK() __debugbreak()
#endif // !DEBUG_BREAK


#define APP_NAMESPACE			PFF
#define PROJECT_FOLDER			"PFF"
#define LOGGER_MACRP_PREFIX		PFF

#define ENABLE_VALIDATION
#define ENABLED_ASSERTS

namespace APP_NAMESPACE {

	namespace Logger {

		enum LogMsgSeverity {
			Trace = 0,
			Debug = 1,
			Info = 2,
			Warn = 3,
			Error = 4,
			Fatal = 5,
			NUM_SEVERITIES = 6
		};
		static const char* SeperatorStringBig = "====================================================================================================================";
		static const char* SeperatorStringSmall = "--------------------------------------------------------------------------------------------------------------------";

		PFF_API int Init(const char* LogCoreFile, const char* LogFile, const char* Format);

		/*  Formating the LogMessages can be customised with the following tags
			to format all following Log Messages use: set_Formating(char* format);
			e.g. set_Formating("$B[$T] $L [$F]  $C$E")  or set_Formating("$BTime:[$M $S] $L $E ==> $C")

		$T		Time				hh:mm:ss
		$H		Hour				hh
		$M		Minute				mm
		$S		Secunde				ss
		$J		MilliSecunde		mm

		$N		Date				yyyy:mm:dd:
		$Y		Date Year			yyyy
		$O		Date Month			mm
		$D		Date Day			dd

		$F		Func. Name			application::main, math::foo
		$P		Func. Name			main, foo
		$A		File Name           C:\Porject\main.c C:\Porject\foo.c
		$K		shortend File Name  Porject\main.c Porject\foo.c
		$I		only File Name      main.c foo.c
		$G		Line				1, 42

		$L		LogLevel			[TRACE], [DEBUG] … [FATAL]
		$X		Alienment			add space for "INFO" & "WARN"
		$B		Color Begin			from here the color starts
		$E		Color End			from here the color ends
		$C		Text				Formated Message with variables
		$Z		New Line			Adds a new Line to the log*/
		void Set_Format(const char* newFormat);
		void Use_Format_Backup();

		// Define witch log levels should be written to log file directly and witch should be buffered
		//  0    =>   write all logs directly to log file
		//  1    =>   buffer: TRACE
		//  2    =>   buffer: TRACE + DEBUG
		//  3    =>   buffer: TRACE + DEBUG + INFO
		//  4    =>   buffer: TRACE + DEBUG + INFO + WARN
		void set_buffer_Level(int newLevel);
		void LogMsg(LogMsgSeverity level, const char* fileName, const char* funcName, int line, const char* message);
		const char* extractFileName(const char* filePath);


		class PFF_API LogMessage : public std::ostringstream {

		public:
			LogMessage(LogMsgSeverity severity, const char* fileName, const char* funcName, int line);
			~LogMessage();

		private:
			LogMsgSeverity m_Severity;
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
#define LOG_CLIENT_LEVEL_ENABLED 4
#define ASSERTS_ENABLED 1
#define MAX_MEASSGE_SIZE 1024



#define THROW_ERR(message)						std::abort();

//  ===================================================================================  Core Logger  ===================================================================================
#ifdef PFF_INSIDE_ENGINE

	#define CORE_LOG_Fatal(message)				{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Fatal,__FILE__,__FUNCTION__,__LINE__).flush() << message; }
	#define CORE_LOG_Error(message)				{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Error,__FILE__,__FUNCTION__,__LINE__).flush() << message; }

	#if CORE_LOG_LEVEL_ENABLED >= 1
	#define CORE_LOG_Warn(message)				{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Warn,__FILE__,__FUNCTION__,__LINE__).flush() << message; }
	#else
	#define CORE_LOG_Warn(message)				{;}
	#endif

	#if CORE_LOG_LEVEL_ENABLED >= 2
	#define CORE_LOG_Info(message)				{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Info,__FILE__,__FUNCTION__,__LINE__).flush() << message; }
	#else
	#define CORE_LOG_Info(message)				{;}
	#endif

	#if CORE_LOG_LEVEL_ENABLED >= 3
	#define CORE_LOG_Debug(message)				{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Debug,__FILE__,__FUNCTION__,__LINE__).flush() << message; }
	#else
	#define CORE_LOG_Debug(message)				{;}
	#endif

	#if CORE_LOG_LEVEL_ENABLED >= 4
	#define CORE_LOG_Trace(message)				{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Trace,__FILE__,__FUNCTION__,__LINE__).flush() << message; }
	#define CORE_LOG_SEPERATOR					APP_NAMESPACE::Logger::Set_Format("$C$Z");										\
													GL_CORE_LOG_Trace(APP_NAMESPACE::Logger::SeperatorStringSmall)				\
													APP_NAMESPACE::Logger::Use_Format_Backup();

	#define CORE_LOG_SEPERATOR_BIG				APP_NAMESPACE::Logger::Set_Format("$C$Z");										\
													GL_CORE_LOG_Trace(APP_NAMESPACE::Logger::SeperatorStringBig)				\
													APP_NAMESPACE::Logger::Use_Format_Backup();
	#else
	#define CORE_LOG_Trace(message, ...)		{;}
	#define CORE_LOG_SEPERATOR					{;}
	#define CORE_LOG_SEPERATOR_BIG				{;}
	#endif

	#define CORE_LOG(severity, message)			CORE_LOG_##severity(message)

	// ---------------------------------------------------------------------------  Assertion & Validation  ---------------------------------------------------------------------------

#ifdef ENABLED_ASSERTS
	#define CORE_ASSERT(expr, successMsg, failureMsg)									\
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
	#define CORE_ASSERT(expr, successMsg, failureMsg)								{expr;}
	#define CORE_ASSERT_S(expr)														{expr;}
#endif // ASSERTS_ENABLED


#ifdef ENABLE_VALIDATION
	#define CORE_VALIDATE(expr, successMsg, failureMsg, ReturnCommand)				\
					if (expr) {														\
						CORE_LOG(Trace, successMsg);								\
					} else {														\
						CORE_LOG(Warn, failureMsg);									\
						ReturnCommand;												\
					}
#else
	#define CORE_VALIDATE(expr, successMsg, failureMsg, ReturnCommand)			{expr;}
#endif // ENABLE_VALIDATION

#endif

//  ===================================================================================  Client Logger  ===================================================================================

#define LOG_Fatal(message)						{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Fatal,__FILE__,__FUNCTION__,__LINE__) << message; }
#define LOG_Error(message)						{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Error,__FILE__,__FUNCTION__,__LINE__) << message; }

#if LOG_LEVEL_ENABLED >= 1
	#define LOG_Warn(message, ...)					{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Warn,__FILE__,__FUNCTION__,__LINE__) << message; }
#else
	#define LOG_Warn(message, ...)					{;}
#endif

#if LOG_LEVEL_ENABLED >= 2
	#define LOG_Info(message, ...)					{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Info,__FILE__,__FUNCTION__,__LINE__) << message; }
#else
	#define LOG_Info(message, ...)					{;}
#endif

#if LOG_LEVEL_ENABLED >= 3
	#define LOG_Debug(message, ...)					{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Debug,__FILE__,__FUNCTION__,__LINE__) << message; }
#else
	#define LOG_Debug(message, ...)					{;}
#endif

#if LOG_LEVEL_ENABLED >= 4
	#define LOG_Trace(message, ...)					{ APP_NAMESPACE::Logger::LogMessage(APP_NAMESPACE::Logger::LogMsgSeverity::Trace,__FILE__,__FUNCTION__,__LINE__) << message; }
#else
	#define LOG_Trace(message, ...)					{;}
#endif

#define LOG(severity, message)					LOG_##severity(message)
