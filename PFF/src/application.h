#pragma once

#include "pch.h"

struct game;

typedef struct {
	i16 start_pos_x;	// Not implemented yet
	i16 start_pos_y;	// Not implemented yet
	u16 start_width;
	u16 start_hight;
	u32 target_fps;		// 0 == fps not limited
	char* name;			// name used in window
} application_config;

PFF_API bool application_create(struct game* game_inst);

PFF_API bool application_run();

bool application_shutdown();
