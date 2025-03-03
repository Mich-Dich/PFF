#pragma once

// =================================================================================  general  ================================================================================

#include <cassert>
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
#include <future>
#include <random>
#include <filesystem>
#include <concepts>

#include <string>
#include <string_view>
#include <cstring>
#include <sstream>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <queue>
#include <span>
#include <list>
#include <map>
#include <deque>
#include <set>
#include <array>
#include <type_traits>
#include <typeinfo>
#include <xmmintrin.h>

#include <stdio.h>
#include <cstdarg>
#include <fstream>
#include <iomanip>

#include <system_error>
#include <signal.h>
#include <csignal>

// ------------------------------- multithreading -------------------------------
#include <condition_variable>
#include <mutex>
#include <atomic>

// ========== glm math ============
#define GLM_ENABLE_EXPERIMENTAL

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/vec3.hpp>							// glm::vec3
#include <glm/vec4.hpp>							// glm::vec4
#include <glm/mat4x4.hpp>						// glm::mat4
#include <glm/ext/matrix_transform.hpp>			// glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp>		// glm::perspective
#include <glm/ext/scalar_constants.hpp>			// glm::pi

// ============================================================================  platform specific  ============================================================================

#ifdef PFF_PLATFORM_WINDOWS
	#include "util/platform/windows_util.h"
	#ifndef NOMINMAX
		#define NOMINMAX				// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
	#endif
#elif defined(PFF_PLATFORM_LINUX)
	#include "util/platform/linux_util.h"
	#include <cxxabi.h>
	// #include <dirent.h>
	// #include <unistd.h>
	// #include <sys/types.h>
	// #include <sys/stat.h>
	#ifndef NOMINMAX
		#define NOMINMAX				// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
	#endif
#else
	#error Unsupported OS detected
#endif


// ------------------------------- independent files (single includes) -------------------------------
#include "util/timing/instrumentor.h"
#include "util/core_config.h"
#include "util/data_types.h"
#include "util/macros.h"

#include "util/io/logger.h"



