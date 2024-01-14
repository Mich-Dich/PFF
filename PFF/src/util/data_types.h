#pragma once

#include <inttypes.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef	float f32;
typedef double f64;

typedef unsigned long long handle;

enum class error_code {
	success = 0,
	generic_not_found,
	error_opening_file,
	line_not_found,
};

enum class system_state {
	active,
	suspended,
	inactive,
};

#define CONFIG_DIR				"./config/"
#define FILE_EXTENSION_CONFIG	".ini"


enum class key_code {

	mouse_moved       = 10,
	mouse_scrolled    = 11,

	key_unknown       = -1,
	key_SPACE         = 32,
	key_APOSTROPHE    = 39,  // '
	key_COMMA         = 44,  // ,
	key_MINUS         = 45,  // -
	key_PERIOD        = 46,  // .
	key_SLASH         = 47,  // /
	key_0             = 48,
	key_1             = 49,
	key_2             = 50,
	key_3             = 51,
	key_4             = 52,
	key_5             = 53,
	key_6             = 54,
	key_7             = 55,
	key_8             = 56,
	key_9             = 57,
	key_SEMICOLON     = 59,  // ;
	key_EQUAL         = 61,  // =
	key_A             = 65,
	key_B             = 66,
	key_C             = 67,
	key_D             = 68,
	key_E             = 69,
	key_F             = 70,
	key_G             = 71,
	key_H             = 72,
	key_I             = 73,
	key_J             = 74,
	key_K             = 75,
	key_L             = 76,
	key_M             = 77,
	key_N             = 78,
	key_O             = 79,
	key_P             = 80,
	key_Q             = 81,
	key_R             = 82,
	key_S             = 83,
	key_T             = 84,
	key_U             = 85,
	key_V             = 86,
	key_W             = 87,
	key_X             = 88,
	key_Y             = 89,
	key_Z             = 90,
	key_LEFT_BRACKET  = 91,  // [
	key_BACKSLASH     = 92,  // \ 
	key_RIGHT_BRACKET = 93,  // ]
	key_GRAVE_ACCENT  = 96,  // `
	key_WORLD_1       = 161, // non-US #1
	key_WORLD_2       = 162, // non-US #2

	// Function keys
	key_ESCAPE        = 256,
	key_ENTER         = 257,
	key_TAB           = 258,
	key_BACKSPACE     = 259,
	key_INSERT        = 260,
	key_DELETE        = 261,
	key_RIGHT         = 262,
	key_LEFT          = 263,
	key_DOWN          = 264,
	key_UP            = 265,
	key_PAGE_UP       = 266,
	key_PAGE_DOWN     = 267,
	key_HOME          = 268,
	key_END           = 269,
	key_CAPS_LOCK     = 280,
	key_SCROLL_LOCK   = 281,
	key_NUM_LOCK      = 282,
	key_PRINT_SCREEN  = 283,
	key_PAUSE         = 284,
	key_F1            = 290,
	key_F2            = 291,
	key_F3            = 292,
	key_F4            = 293,
	key_F5            = 294,
	key_F6            = 295,
	key_F7            = 296,
	key_F8            = 297,
	key_F9            = 298,
	key_F10           = 299,
	key_F11           = 300,
	key_F12           = 301,
	key_F13           = 302,
	key_F14           = 303,
	key_F15           = 304,
	key_F16           = 305,
	key_F17           = 306,
	key_F18           = 307,
	key_F19           = 308,
	key_F20           = 309,
	key_F21           = 310,
	key_F22           = 311,
	key_F23           = 312,
	key_F24           = 313,
	key_F25           = 314,
	key_KP_0          = 320,
	key_KP_1          = 321,
	key_KP_2          = 322,
	key_KP_3          = 323,
	key_KP_4          = 324,
	key_KP_5          = 325,
	key_KP_6          = 326,
	key_KP_7          = 327,
	key_KP_8          = 328,
	key_KP_9          = 329,
	key_KP_DECIMAL    = 330,
	key_KP_DIVIDE     = 331,
	key_KP_MULTIPLY   = 332,
	key_KP_SUBTRACT   = 333,
	key_KP_ADD        = 334,
	key_KP_ENTER      = 335,
	key_KP_EQUAL      = 336,
	key_LEFT_SHIFT    = 340,
	key_LEFT_CONTROL  = 341,
	key_LEFT_ALT      = 342,
	key_LEFT_SUPER    = 343,
	key_RIGHT_SHIFT   = 344,
	key_RIGHT_CONTROL = 345,
	key_RIGHT_ALT     = 346,
	key_RIGHT_SUPER   = 347,
	key_MENU          = 348,

};
