#pragma once

#include "core/pch.h"
#include "render/render.h"

typedef struct global {
	Render_State render;
	Config_State config;
	Input_State input;
	Time_State time;
    Render_State render;
} Global;

extern Global global;
