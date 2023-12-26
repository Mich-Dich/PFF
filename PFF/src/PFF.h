#pragma once

#ifdef PFF_PLATFORM_WINDOWS
	#ifdef PFF_INSIDE_ENGINE
	#error <PFF.h> should only be included by client appllications
	#endif // PFF_INSIDE_ENGINE
#endif // PFF_PLATFORM_WINDOWS

#include "engine/types.h"

#include "engine/application.h"
#include "engine/EntryPoint.h"				// <- Entry Point of engine application
#include "engine/core/Logger.h"
#include "engine/core/window.h"