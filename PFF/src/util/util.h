#pragma once

#include <stdbool.h>

#include "types.h"
#include "dll_interface.h"

#include "pff_time.h"
#include "pff_math.h"

#include "container/c_vec.h"

#include "IOCore/IOCore.h"
#include "IOCore/logger.h"

// commend templates

// ------------------------------------------------------------------------------------------ static vaiables -----------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------ privete funcs  ------------------------------------------------------------------------------------------

// ========================================================================================== main functions ==========================================================================================

#define WHITE			(vec4){1, 1, 1, 1}
#define BLACK			(vec4){0, 0, 0, 1}
#define RED				(vec4){1, 0, 0, 1}
#define GREEN			(vec4){0, 1, 0, 1}
#define GREEN_DARK		(vec4){0, 0.5f, 0, 1}
#define GREEN_DARK_02	(vec4){0, 0.3f, 0, 1}
#define BLUE			(vec4){0, 0, 1, 1}
#define YELLOW			(vec4){1, 1, 0, 1}
#define CYAN			(vec4){0, 1, 1, 1}
#define MAGENTA			(vec4){1, 0, 1, 1}
#define ORANGE			(vec4){1, 0.5f, 0, 1}
#define PURPLE			(vec4){0.5f, 0, 1, 1}
#define TURQUOISE		(vec4){0, 1, 0.5f, 1}

static inline char* bool_to_str(bool value) { return (value) ? " true" : "false"; }

static inline char* ptr_to_str(void* value) { return (value != NULL) ? "  valid" : "invalid"; }

typedef enum {
	EC_success = 0,
	EC_generic_not_found,
	EC_error_opening_file,
	EC_line_not_found,
} error_code;