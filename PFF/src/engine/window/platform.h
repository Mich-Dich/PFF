#pragma once

#include "pch.h"


struct SDL_Window;

struct SDL_Window* create_window(const char* title, u32 window_width, u32 window_height);
