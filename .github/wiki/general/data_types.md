# Data Types

This page outlines the intrinsic data types used throughout the game engine. These types are fundamental to the engine's operation and are defined to ensure consistency and ease of use.

 ## Primitive Types
  The game engine uses a variety of primitive types for different purposes. Here are the defined types:

  Unsigned integer types
  `u8`   is a uint8_t
  `u16` is a uint16_t
  `u32` is a uint32_t
  `u64` is a uint64_t
  
  Signed integer types
  `int8`   is a int8_t
  `int16` is a int16_t
  `int32` is a int32_t
  `int64` is a int64_t
  
  Floating-point types
  `f32`   is a float
  `f64`   is a double
  `f128` is a long double
  
  Handle type (typically used for object handles or identifiers)
  `handle` is a unsigned long long

## Enums

 ### `error_code`
  The `error_code` enum defines various error codes that the engine might encounter.
 
  ```cpp
enum class error_code {
  success = 0,          // No error
  generic_not_found,    // General not found error
  error_opening_file,   // Error opening a file
  line_not_found,       // Line not found in file
};
   ```
   
 ### system_state
  The `system_state` enum defines the possible states of the system.
  
  ```cpp
enum class system_state {
  active,    // System is active and running
  suspended, // System is paused or suspended
  inactive,  // System is inactive or shut down
};
  ```

 ## Configuration Constants
  The following constants are used for file configurations:

  ```cpp
#define CONFIG_DIR                "./config/"  // Directory for configuration files
#define FILE_EXTENSION_CONFIG     ".yml"       // Extension for YAML configuration files
#define FILE_EXTENSION_INI        ".ini"       // Extension for INI configuration files
  ```
