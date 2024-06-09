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
typedef long double f128;

typedef unsigned long long handle;

struct system_time {

	unsigned short year;
	unsigned short month;
	unsigned short day_of_week;
	unsigned short day;
	unsigned short hour;
	unsigned short minute;
	unsigned short secund;
	unsigned short millisecends;
};

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
#define FILE_EXTENSION_CONFIG	".yml"

enum class key_state {
	release = 0,
	press	= 1,
	repeat	= 2,
};

enum class key_code {

	mouse_bu_1			= 0,
	mouse_bu_2			= 1,
	mouse_bu_3			= 2,
	mouse_bu_4			= 3,
	mouse_bu_5			= 4,
	mouse_bu_6			= 5,
	mouse_bu_7			= 6,
	mouse_bu_8			= 7,
	mouse_bu_last		= mouse_bu_8,
	mouse_bu_left		= mouse_bu_1,
	mouse_bu_right		= mouse_bu_2,
	mouse_bu_middle		= mouse_bu_3,

	mouse_moved			= 110,
	mouse_moved_x		= 111,
	mouse_moved_y		= 112,
	mouse_scrolled_x	= 113,
	mouse_scrolled_y	= 114,

	key_unknown			= -1,
	key_space			= 32,
	key_apostrophe		= 39,  // '
	key_comma			= 44,  // ,
	key_minus			= 45,  // -
	key_period			= 46,  // .
	key_slash			= 47,  // /
	key_0				= 48,
	key_1				= 49,
	key_2				= 50,
	key_3				= 51,
	key_4				= 52,
	key_5				= 53,
	key_6				= 54,
	key_7				= 55,
	key_8				= 56,
	key_9				= 57,
	key_semicolon		= 59,  // ;
	key_equal			= 61,  // =
	key_A				= 65,
	key_B				= 66,
	key_C				= 67,
	key_D				= 68,
	key_E				= 69,
	key_F				= 70,
	key_G				= 71,
	key_H				= 72,
	key_I				= 73,
	key_J				= 74,
	key_K				= 75,
	key_L				= 76,
	key_M				= 77,
	key_N				= 78,
	key_O				= 79,
	key_P				= 80,
	key_Q				= 81,
	key_R				= 82,
	key_S				= 83,
	key_T				= 84,
	key_U				= 85,
	key_V				= 86,
	key_W				= 87,
	key_X				= 88,
	key_Y				= 89,
	key_Z				= 90,
	key_left_bracket	= 91,  // [
	key_backslach		= 92,  // \ 
	key_right_bracket	= 93,  // ]
	key_grave_accent	= 96,  // `
	key_world_1			= 161, // non-US #1
	key_world_2			= 162, // non-US #2

	// Function keys
	key_escape			= 256,
	key_enter			= 257,
	key_tab				= 258,
	key_backspace		= 259,
	key_insert			= 260,
	key_delete			= 261,
	key_right			= 262,
	key_left			= 263,
	key_down			= 264,
	key_up				= 265,
	key_page_up			= 266,
	key_page_down		= 267,
	key_home			= 268,
	key_end				= 269,
	key_caps_lock		= 280,
	key_scroll_lock		= 281,
	key_num_lock		= 282,
	key_print_screen	= 283,
	key_pause			= 284,
	key_F1				= 290,
	key_F2				= 291,
	key_F3				= 292,
	key_F4				= 293,
	key_F5				= 294,
	key_F6				= 295,
	key_F7				= 296,
	key_F8				= 297,
	key_F9				= 298,
	key_F10				= 299,
	key_F11				= 300,
	key_F12				= 301,
	key_F13				= 302,
	key_F14				= 303,
	key_F15				= 304,
	key_F16				= 305,
	key_F17				= 306,
	key_F18				= 307,
	key_F19				= 308,
	key_F20				= 309,
	key_F21				= 310,
	key_F22				= 311,
	key_F23				= 312,
	key_F24				= 313,
	key_F25				= 314,
	key_kp_0			= 320,
	key_kp_1			= 321,
	key_kp_2			= 322,
	key_kp_3			= 323,
	key_kp_4			= 324,
	key_kp_5			= 325,
	key_kp_6			= 326,
	key_kp_7			= 327,
	key_kp_8			= 328,
	key_kp_9			= 329,
	key_kp_decimal		= 330,
	key_kp_divide		= 331,
	key_kp_multiply		= 332,
	key_kp_subtrace		= 333,
	key_kp_add			= 334,
	key_kp_enter		= 335,
	key_kp_equal		= 336,
	key_left_shift		= 340,
	key_left_control	= 341,
	key_left_alt		= 342,
	key_left_super		= 343,	// windows key
	key_right_shift		= 344,
	key_right_control	= 345,
	key_right_alt		= 346,
	key_right_super		= 347,	// windows key
	key_menu			= 348,

};
