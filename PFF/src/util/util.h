#pragma once

#define CONSTEXPR				constexpr

#define ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))


#ifdef PFF_PLATFORM_WINDOWS
	#include "platform/windows_util.h"
#else
	#error PFF only suports windows
#endif

#include "util/logger.h"
#include "util/data_types.h"
#include "util/io/io_handler.h"
#include "util/io/config.h"

namespace PFF {


}