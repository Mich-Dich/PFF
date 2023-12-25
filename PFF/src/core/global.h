#pragma once

#include "core/pch.h"

#include "render/render.h"
#include "core/utils/pff_time.h"
#include "core/config.h"
#include "core/input.h"

typedef struct global {
	Render_State render;
	Config_State config;
	input_State input;
	Time_State time;
	c_vector Input_Mappings;
} Global;

extern Global global;
