# PFF Logging System
 
 The PFF Logging System provides a flexible and powerful way to handle logging and debugging output in your application. It supports multiple log levels, customizable log formatting, and the ability to control which messages are buffered or written directly to the log file. Below is a guide on how to use the logging system effectively.
 
 ## Logging Messages
  Use the `LogMsg` function to log messages directly with a specified severity:
  ```cpp
u32 test_var = 42;
LO(Info, "This is an info message, example parameter: "<< test_var);
  ```

  Alternatively, use the `LogMessage` class for a more streamlined approach:
  ```cpp
PFF::logger::LogMessage(PFF::logger::LogMsgSeverity::Error, __FILE__, __FUNCTION__, __LINE__) << "This is an error message.";
  ```
   
  ## Assertions and Validations

  The logging system includes macros for assertions and validation checks:
    
  - `CORE_ASSERT(expr, successMsg, failureMsg)`: Logs a message and breaks if the expression evaluates to false (core logs).
  - `CORE_VALIDATE(expr, ReturnCommand, successMsg, failureMsg)`: Logs a message and executes `ReturnCommand` if the expression is false (core logs).
  - `LOG_ASSERT(expr, successMsg, failureMsg)`: Similar to `CORE_ASSERT` but for client-side logs.
  - `LOG_VALIDATE(expr, ReturnCommand, successMsg, failureMsg)`: Similar to `CORE_VALIDATE` but for client-side logs.
    
  ** Usage: **

  Here’s how you might use the logging system in practice:

  Log messages of various severities
  ```cpp
LOG(Info, "informative message.");
LOG(Error, "An error occurred in the application.");
  ```

  An assert will execute the  cause a debugbreak if the expression is false
  ```cpp
ASSERT(x > 0, "x is positive", "x is not positive");
  ```

  A validation will execute the [ReturnCommand] if the expression is false, in this case [return]
  ```cpp
VALIDATE(y < 10, return, "y is less than 10", "y is greater than or equal to 10");
  ```

  ## Log Levels
   
  Log messages hav diffrent severity levels:
   Trace < Debug < Info < Warn < Error < Fatal

  Control which log levels are enabled using the following definitions:
   
  - `LOG_LEVEL_ENABLED`: Set the log level for client-side logs. For example, `#define LOG_LEVEL_ENABLED 4` enables FATAL, ERROR, WARN, INFO, DEBUG, and TRACE levels.
   - 0    =>   FATAL + ERROR
   - 1    =>   FATAL + ERROR + WARN
   - 2    =>   FATAL + ERROR + WARN + INFO
   - 3    =>   FATAL + ERROR + WARN + INFO + DEBUG
   - 4    =>   FATAL + ERROR + WARN + INFO + DEBUG + TRACE
   
  ## Log Formatting

   Log messagen are formatted using special tags in the format string. Tags are replaced with relevant information when a log message is created. Here are some of the available tags:
   The default format is "[$B$T:$J$E] [$B$L$X $I - $P:$G$E] $C$Z"
 
   - `$T` - Time in `hh:mm:ss` format
   - `$H` - Hour (`hh`)
   - `$M` - Minute (`mm`)
   - `$S` - Second (`ss`)
   - `$J` - Millisecond (`mmm`)
   - `$N` - Date in `yyyy/mm/dd` format
   - `$Y` - Year (`yyyy`)
   - `$O` - Month (`mm`)
   - `$D` - Day (`dd`)
   - `$F` - Function name (full)
   - `$P` - Function name (short)
   - `$A` - Full file name
   - `$K` - Shortened file name
   - `$I` - Only file name
   - `$G` - Line number
   - `$L` - Log level (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)
   - `$C` - Message content
   - `$B` - Color start (ANSI escape code)
   - `$E` - Color end (ANSI escape code)
   - `$X` - Alignment for certain log levels
   - `$Z` - New line
   
   To change the format for all following messages, use:
   ```cpp
PFF::logger::set_format("$B[$T] $L $C$E");
   ```

   To revert to the previous format, use:
   ```cpp
PFF::logger::use_format_backup();
   ```

  ## Buffering Log Messages
   
   Decide which log levels should be buffered before writing to the file. The `set_buffer_Level` function allows you to specify the maximum log level that should be buffered:
   
   ```cpp
PFF::logger::set_buffer_Level(3); // Buffers TRACE, DEBUG, INFO, and WARN; writes ERROR and FATAL directly
   ```
 