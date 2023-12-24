#pragma once

#include "core/pch.h"
#include "render/render.h"

#include "core/config.h"
#include "core/input.h"
#include "core/types.h"
#include "core/logger.h"

typedef struct global {
	Render_State render;
	Config_State config;
	Input_State input;
	Time_State time;
} Global;

extern Global global;
