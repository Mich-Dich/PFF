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

#define FILE_EXTENSION_CONFIG ".ini"
