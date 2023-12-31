#pragma once

// =================================================================================  general  ================================================================================

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <cstdio>
#include <chrono>
#include <thread>

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
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

// ============================================================================  platform specific  ============================================================================

#ifdef PFF_PLATFORM_WINDOWS

	//#include <Windows.h>

#else
	#error PFF only suports windows
#endif

// ==================================================================================  utils  ==================================================================================

#include "util/util.h"

// ==================================================================================  utils  ==================================================================================

#include "util/math/constance.h"

// =================================================================================  macros  ==================================================================================

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
