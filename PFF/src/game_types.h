#pragma once

#include "pch.h"
#include "application.h"

typedef struct game {

	application_config app_config;
	bool (*initalize)(struct game* game_inst);
	bool (*update)(struct game* game_inst, f64 delta_time);
	bool (*render)(struct game* game_inst, f64 delta_time);
	void (*on_resize)(struct game* game_inst, u32 width, u32 height);
	void* state;

} game;
