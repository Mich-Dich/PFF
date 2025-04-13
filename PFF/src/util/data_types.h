#pragma once

#include <inttypes.h>

// =============================================
// @brief Primitive type definitions for consistent sizing across platforms
// =============================================

typedef uint8_t  u8;    							///< 8-bit unsigned integer
typedef uint16_t u16;   							///< 16-bit unsigned integer
typedef uint32_t u32;   							///< 32-bit unsigned integer
typedef uint64_t u64;   							///< 64-bit unsigned integer

typedef int8_t  int8;   							///< 8-bit signed integer
typedef int16_t int16;  							///< 16-bit signed integer
typedef int32_t int32;  							///< 32-bit signed integer
typedef int64_t int64;  							///< 64-bit signed integer

typedef float f32;          						///< 32-bit floating point
typedef double f64;         						///< 64-bit floating point
typedef long double f128;   						///< 128-bit floating point (platform dependent)

// Platform-specific types				
typedef unsigned long long handle;  				///< Generic handle type for OS resources

#define PROJECT_PATH				application::get().get_project_path()
#define PROJECT_NAME				application::get().get_project_data().name

// Extension for asset files
#define PFF_ASSET_EXTENTION      	".pffasset"

// Extension for project files
#define PFF_PROJECT_EXTENTION    	".pffproj"      

// Temporary directory for DLL builds
#define PFF_PROJECT_TEMP_DLL_PATH 	"_build_DLL"    

// Directory structure macros
#define METADATA_DIR            	"metadata"      ///< Directory for metadata files
#define CONFIG_DIR              	"config"        ///< Directory for configuration files
#define CONTENT_DIR             	"content"       ///< Directory for content files
#define SOURCE_DIR              	"src"           ///< Directory for source code

// Configuration file extensions
#define FILE_EXTENSION_CONFIG   	".yml"        	///< Extension for YAML config files
#define FILE_EXTENSION_INI      	".ini"          ///< Extension for INI config files

namespace PFF {

    // =============================================
    // @brief Type traits and utility templates
    // =============================================

    // @brief Type trait to detect std::vector types
    template<typename T>
    struct is_vector : std::false_type {};

    template<typename T, typename Alloc>
    struct is_vector<std::vector<T, Alloc>> : std::true_type {};

    // @brief Fixed-size character array type
    template <size_t N>
    struct char_array { char data[N]; };

    // =============================================
    // @brief Smart pointer utilities
    // =============================================

    // @brief [ref] is a reference-counted smart pointer similar to std::shared_ptr
    template<typename T>
    using ref = std::shared_ptr<T>;

    // @brief Creates a reference-counted object with perfect forwarding
    template<typename T, typename ... args>
    constexpr ref<T> create_ref(args&& ... arguments) {
        return std::make_shared<T>(std::forward<args>(arguments)...);
    }

    // @brief [scope_ref] is an owning smart pointer similar to std::unique_ptr
    template<typename T>
    using scope_ref = std::unique_ptr<T>;

    // @brief Creates a scoped object with perfect forwarding
    template<typename T, typename ... args>
    constexpr scope_ref<T> create_scoped_ref(args&& ... arguments) {
        return std::make_unique<T>(std::forward<args>(arguments)...);
    }
	
	// ------------------------ smart pointers ------------------------

    // @brief Represents a 3D extent (width, height, depth)
    typedef struct extent_3D {
        u32 width;      		// Width component
        u32 height;     		// Height component
        u32 depth;      		// Depth component
    } extent_3D;

    // @brief Semantic versioning structure
	struct version {

		version() {}
		version(u16 major, u16  minor, u16  patch)
			:major(major), minor(minor), patch(patch) {}

		u16 major{};    		// Major version number (incompatible API changes)
		u16 minor{};    		// Minor version number (backwards-compatible functionality)
		u16 patch{};    		// Patch version number (backwards-compatible bug fixes)

        // @brief Converts version to string in "major:minor:patch" format
		std::string to_str() const { return std::format("{}:{}:{}", major, minor, patch); }
        
        // @brief Implicit conversion to string_view
		operator std::string_view () { return std::format("{}:{}:{}", major, minor, patch); }
	};
	
    // @brief Stream output operator for version
	inline std::ostream& operator<<(std::ostream& os, const version& v) { return os << v.to_str(); }

    // @brief System time representation
	struct system_time {

        u16 year;           	// Full year (e.g., 2025)
        u8 month;           	// Month (1-12)
        u8 day;             	// Day of month (1-31)
        u8 day_of_week;     	// Day of week (0-6, where 0=Sunday)
        u8 hour;            	// Hour (0-23)
        u8 minute;          	// Minute (0-59)
        u8 secund;          	// Second (0-59)
        u16 millisecend;    	// Millisecond (0-999)

        // @brief Converts system_time to human-readable string
		std::string to_str() const { return std::format("{}-{}-{} ({}) {}:{}:{}:{}", year, month, day, day_of_week, hour, minute, secund, millisecend); }
	};

    // =============================================
    // @brief Enumerations
    // =============================================

    // @brief Precision levels for duration measurements
    enum class duration_precision : u8 {
        microseconds,    		// Microsecond precision
        milliseconds,    		// Millisecond precision
        seconds,         		// Second precision
    };

    // @brief System error codes
    enum class error_code : u8 {
        success = 0,            // Operation succeeded
        generic_not_found,      // Generic "not found" error
        file_not_found,         // File not found
        error_opening_file,     // Error opening file
        system_path_not_free,   // System path in use
        line_not_found,         // Line not found in file
    };

    // @brief System state enumeration
    enum class system_state : u8 {
        active,     			// System is fully operational
        suspended,  			// System is paused but retains state
        inactive,   			// System is shut down
    };

    // @brief Key state enumeration (for input handling)
    enum class key_state : u8 {
        release = 0,    		// Key was released
        press = 1,      		// Key was pressed
        repeat = 2,     		// Key is being held down (repeat)
    };

    // @brief Key code enumeration (keyboard/mouse input)
	enum class key_code {

		mouse_bu_1 = 0,
		mouse_bu_2 = 1,
		mouse_bu_3 = 2,
		mouse_bu_4 = 3,
		mouse_bu_5 = 4,
		mouse_bu_6 = 5,
		mouse_bu_7 = 6,
		mouse_bu_8 = 7,
		mouse_bu_last = mouse_bu_8,
		mouse_bu_left = mouse_bu_1,
		mouse_bu_right = mouse_bu_2,
		mouse_bu_middle = mouse_bu_3,

		mouse_moved = 110,
		mouse_moved_x = 111,
		mouse_moved_y = 112,
		mouse_scrolled_x = 113,
		mouse_scrolled_y = 114,

		key_unknown = -1,
		key_space = 32,
		key_apostrophe = 39,  			// '
		key_comma = 44,  				// ,
		key_minus = 45,  				// -
		key_period = 46, 				// .
		key_slash = 47,  				// /
		key_0 = 48,
		key_1 = 49,
		key_2 = 50,
		key_3 = 51,
		key_4 = 52,
		key_5 = 53,
		key_6 = 54,
		key_7 = 55,
		key_8 = 56,
		key_9 = 57,
		key_semicolon = 59,  			// ;
		key_equal = 61,  				// =
		key_A = 65,
		key_B = 66,
		key_C = 67,
		key_D = 68,
		key_E = 69,
		key_F = 70,
		key_G = 71,
		key_H = 72,
		key_I = 73,
		key_J = 74,
		key_K = 75,
		key_L = 76,
		key_M = 77,
		key_N = 78,
		key_O = 79,
		key_P = 80,
		key_Q = 81,
		key_R = 82,
		key_S = 83,
		key_T = 84,
		key_U = 85,
		key_V = 86,
		key_W = 87,
		key_X = 88,
		key_Y = 89,
		key_Z = 90,
		key_backslach = 91,				/* \ */ 
		key_left_bracket = 92,			// [
		key_right_bracket = 93,			// ]
		key_grave_accent = 96,			// `
		key_world_1 = 161,				// non-US #1
		key_world_2 = 162,				// non-US #2

		// Function keys
		key_escape = 256,
		key_enter = 257,
		key_tab = 258,
		key_backspace = 259,
		key_insert = 260,
		key_delete = 261,
		key_right = 262,
		key_left = 263,
		key_down = 264,
		key_up = 265,
		key_page_up = 266,
		key_page_down = 267,
		key_home = 268,
		key_end = 269,
		key_caps_lock = 280,
		key_scroll_lock = 281,
		key_num_lock = 282,
		key_print_screen = 283,
		key_pause = 284,
		key_F1 = 290,
		key_F2 = 291,
		key_F3 = 292,
		key_F4 = 293,
		key_F5 = 294,
		key_F6 = 295,
		key_F7 = 296,
		key_F8 = 297,
		key_F9 = 298,
		key_F10 = 299,
		key_F11 = 300,
		key_F12 = 301,
		key_F13 = 302,
		key_F14 = 303,
		key_F15 = 304,
		key_F16 = 305,
		key_F17 = 306,
		key_F18 = 307,
		key_F19 = 308,
		key_F20 = 309,
		key_F21 = 310,
		key_F22 = 311,
		key_F23 = 312,
		key_F24 = 313,
		key_F25 = 314,
		key_kp_0 = 320,
		key_kp_1 = 321,
		key_kp_2 = 322,
		key_kp_3 = 323,
		key_kp_4 = 324,
		key_kp_5 = 325,
		key_kp_6 = 326,
		key_kp_7 = 327,
		key_kp_8 = 328,
		key_kp_9 = 329,
		key_kp_decimal = 330,
		key_kp_divide = 331,
		key_kp_multiply = 332,
		key_kp_subtrace = 333,
		key_kp_add = 334,
		key_kp_enter = 335,
		key_kp_equal = 336,
		key_left_shift = 340,
		key_left_control = 341,
		key_left_alt = 342,
		key_left_super = 343,			// windows key
		key_right_shift = 344,
		key_right_control = 345,
		key_right_alt = 346,
		key_right_super = 347,			// windows key
		key_menu = 348,

	};

}
