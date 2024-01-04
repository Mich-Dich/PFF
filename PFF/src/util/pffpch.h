#pragma once

// =================================================================================  general  ================================================================================

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <list>
#include <map>
#include <set>
#include <array>
#include <type_traits>

#include <stdio.h>
#include <cstdarg>
#include <fstream>
#include <iomanip>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// ============================================================================  platform specific  ============================================================================

#ifdef PFF_PLATFORM_WINDOWS

	//#include <Windows.h>
	#include "platform/windows_util.h"

#else
	#error PFF only suports windows
#endif

// ==================================================================================  utils  ==================================================================================

#include "logger.h"
#include "data_types.h"
#include "util/io/io_handler.h"
#include "util/io/config.h"

// =================================================================================  macros  ==================================================================================

#define APP_NAMESPACE PFF

#define BIT(x) (1 << x)

#define STD_BIND_EVENT_FN(x)				std::bind(&x, this, std::placeholders::_1)

#define DELETE_COPY(type)					type(const type&) = delete;								\
											type& operator=(const type&) = delete

#define DELETE_MOVE(type)					type(type&&) = delete;									\
											type& operator=(type&&) = delete

#define DELETE_COPY_MOVE(type)				type(const type&) = delete;								\
											type& operator=(const type&) = delete;					\
											type(type&&) = delete;									\
											type& operator=(type&&) = delete

// ================================================================================  functions  ================================================================================

template <typename T>
T str_to_num(const std::string& str) {
	std::istringstream ss(str);
	T num{};
	ss >> num;
	return num;
}

template <typename T>
std::string num_to_str(const T& num) {
	std::ostringstream oss;
	oss << num;
	return oss.str();
}

FORCEINLINE bool str_to_bool(const std::string& string) { return(string == "true") ? true : false; }
FORCEINLINE const char* bool_to_str(bool boolean) { return boolean ? "true" : "false"; }
