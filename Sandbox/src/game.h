#pragma once

#include <PFF.h>

typedef struct {
	f64 delta_time;
} game_state;



bool game_initalize(game* game_inst);

bool game_update(game* game_inst, f64 delta_time);

bool game_render(game* game_inst, f64 delta_time);

void game_on_resize(game* game_inst, u32 width, u32 height);
