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

PFF_API bool application_run(void);

PFF_API f32 get_delta_time(void);
PFF_API u32 get_window_width(void);
PFF_API u32 get_window_height(void);
PFF_API bool is_courser_in_window(void);
PFF_API void get_courser_pos(i32* x, i32* y);
PFF_API void QuitGame(void);

bool application_shutdown(void);
