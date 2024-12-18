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
//#include <concepts>			// need to switch to C++20

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <optional>
#include <queue>
//#include <span>
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
#elif defined(PFF_PLATFORM_LINUX)
	#error PFF only suports windows
	#include "util/platform/linux_util.h"
#else
	#error Unsupported OS detected
#endif


// ------------------------------- independent files (single includes) -------------------------------
#include "util/timing/instrumentor.h"
#include "util/core_config.h"
#include "util/data_types.h"
#include "util/macros.h"

#include "util/io/logger.h"
