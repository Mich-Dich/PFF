#pragma once

#include "pch.h"


struct SDL_Window;

struct SDL_Window* window_create(const char* title, u32 window_width, u32 window_height);
void window_update(void);
