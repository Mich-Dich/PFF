#pragma once

// =================================================================================  includes  ==================================================================================

#include "util/logger.h"
#include "util/io/io_handler.h"
#include "util/io/config.h"
#include "util/data_types.h"

// =================================================================================  macros  ==================================================================================

#define CONSTEXPR				constexpr

#define ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

#define APP_NAMESPACE PFF

#define BIT(x) (1 << x)

#define BIND_FN(x)							std::bind(&x, this, std::placeholders::_1)

#define DELETE_COPY(type)					type(const type&) = delete;								\
											type& operator=(const type&) = delete

#define DELETE_MOVE(type)					type(type&&) = delete;									\
											type& operator=(type&&) = delete

#define DELETE_COPY_MOVE(type)				type(const type&) = delete;								\
											type& operator=(const type&) = delete;					\
											type(type&&) = delete;									\
											type& operator=(type&&) = delete


