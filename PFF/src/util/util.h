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


#define APP_NAMESPACE PFF

#ifdef PFF_PLATFORM_WINDOWS

	//#include <Windows.h>
	#include "platform/windows_util.h"

#else
	#error PFF only suports windows
#endif

#define BIT(x) (1 << x)


// ======================================  utils  ======================================
#include "logger.h"
#include "data_types.h"
#include "util/io_handler.h"


#define DELETE_COPY(classname)				classname(const classname&) = delete;							\
											classname& operator=(const classname&) = delete

#define DELETE_COPY_MOVE(classname)			classname(const classname&) = delete;							\
											classname& operator=(const classname&) = delete;						\
											classname(classname&&) = delete;								\
											classname& operator=(classname&&) = delete


#define GENERATE_CLASS_BODY					static const char* getClassName() { \
												static const std::string className = __PRETTY_FUNCTION__; \
												size_t start = className.find("class ") + 6; \
												size_t end = className.find(" ", start); \
												return className.substr(start, end - start).c_str(); \
											}

#define STD_BIND_EVENT_FN(x)				std::bind(&x, this, std::placeholders::_1)
