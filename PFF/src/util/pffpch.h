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
#include <deque>
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
#include <glm/gtc/random.hpp>

// ============================================================================  platform specific  ============================================================================

#ifdef PFF_PLATFORM_WINDOWS
	//#include <Windows.h>
	#include "util/platform/windows_util.h"
#else
	#error PFF only suports windows
#endif

// ==================================================================================  utils  ==================================================================================

#include "util/util.h"
#include "util/math/constance.h"
#include "util/math/random.h"
//#include "util/math/random.h"

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
