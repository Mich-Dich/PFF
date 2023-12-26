#pragma once

#ifdef PFF_PLATFORM_WINDOWS
	#ifdef PFF_INSIDE_ENGINE
		#define PFF_API __declspec(dllexport)
	#else
		#define PFF_API __declspec(dllimport)
	#endif // PFF_INSIDE_ENGINE
#else
	#error PFF is Windows-only for now
#endif // PFF_PLATFORM_WINDOWS

#define BIT(x) (1 << x)

#define GL_BIND_EVENT_FN(func) std::bind(&func, this, std::placeholders::_1)