#pragma once

#include "pch.h"

#include "engine/render/render.h"
#include "engine/config.h"
#include "engine/input.h"

#include "util/util.h"

typedef struct global {
	Render_State render;
	Config_State config;
	input_State input;
	time_info time;
	c_vector Input_Mappings;
} Global;

extern Global global;
