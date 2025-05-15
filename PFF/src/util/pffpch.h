#pragma once


#ifndef NOMINMAX
	#define NOMINMAX				// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#endif

// ======================================================= general =======================================================

// Core Language Features
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <concepts>

// Strings and Text Manipulation
#include <string>
#include <string_view>
#include <cstring>
#include <sstream>
#include <regex>
#include <iomanip>

// Input/Output and Filesystem
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <stdio.h>

// Data Structures and Containers
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Multithreading and Concurrency
#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>

// Time and Randomness
#include <chrono>
#include <random>

// System and Low-Level Utilities
#include <system_error>
#include <csignal>
#include <signal.h>
#include <xmmintrin.h>

// Variadic and Debugging Utilities
#include <cstdarg>
#include <cassert>
#include <cstdlib>

// glm math
#define GLM_ENABLE_EXPERIMENTAL

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>			// glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp>		// glm::perspective
#include <glm/ext/scalar_constants.hpp>			// glm::pi

// ======================================================= platform specific  =======================================================

#ifdef PFF_PLATFORM_WINDOWS
	#include "util/platform/windows_util.h"
#elif defined(PFF_PLATFORM_LINUX)
	#include "util/platform/linux_util.h"
	#include <cxxabi.h>
#else
	#error Unsupported OS detected
#endif


// ------------------------- independent files (single includes)-------------------------
#include "util/core_config.h"
#include "util/data_types.h"
#include "util/macros.h"
#include "util/timing/instrumentor.h"
#include "util/io/logger.h"

#include "util/data_structures/path_manipulation.h"
