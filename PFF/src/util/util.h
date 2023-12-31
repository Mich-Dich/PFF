#pragma once

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
#include "logger.h"
#include "data_types.h"
#include "util/io_handler.h"


#define DELETE_COPY_CONSTRUCTOR				pff_window(const pff_window&) = delete;							\
											pff_window& operator=(const pff_window&) = delete;

#define GENERATE_CLASS_BODY					static const char* getClassName() { \
												static const std::string className = __PRETTY_FUNCTION__; \
												size_t start = className.find("class ") + 6; \
												size_t end = className.find(" ", start); \
												return className.substr(start, end - start).c_str(); \
											}
