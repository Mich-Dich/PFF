#pragma once

#include "core/pch.h"
#include "render/render.h"

#include "core/utils/pff_time.h"
#include "core/config.h"

typedef struct global {
	Render_State render;
	Config_State config;
	input_State input;
	Time_State time;
} Global;

extern Global global;
