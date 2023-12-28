#pragma once


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
#include <type_traits>

#include <stdio.h>
#include <cstdarg>
#include <fstream>
#include <iomanip>

#ifdef GL_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#define APP_NAMESPACE PFF

#ifdef PFF_PLATFORM_WINDOWS
	#ifdef PFF_INSIDE_ENGINE
		#define PFF_API __declspec(dllexport)
	#else
		#define PFF_API __declspec(dllimport)
	#endif // PFF_INSIDE_ENGINE
#endif // PFF_PLATFORM_WINDOWS

#define BIT(x) (1 << x)


// ======================================  utils  ======================================
#include "Logger.h"
#include "data_types.h"

