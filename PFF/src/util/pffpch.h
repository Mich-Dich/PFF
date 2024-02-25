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

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <queue>
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
#include "util/benchmarking/instrumentor.h"
//#include "util/math/random.h"
