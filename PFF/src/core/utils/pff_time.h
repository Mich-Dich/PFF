#pragma once

#include "core/types.h"

typedef struct time_state {
	f32 delta;			// time between frames
	f32 now;			// time of current frame
	f32 last;			// time of last frame

	f32 frame_last;		// last time FPS was calculated
	f32 frame_delay;	// max time a frame should take (target FPS)
	f32 frame_time;		// time the current frame took

	u32 frame_rate;		// current FPS
	u32 frame_count;	// helps calculate FPS
} Time_State;

void time_init(u32 frame_rate);
void time_update(void);
void time_update_late(void);