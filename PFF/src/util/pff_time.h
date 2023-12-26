#pragma once

#include "util/types.h"

typedef struct {
	f32 delta;			// time between frames
	f32 now;			// time of current frame
	f32 last;			// time of last frame

	f32 frame_last;		// last time FPS was calculated
	f32 frame_delay;	// max time a frame should take (target FPS)
	f32 frame_time;		// time the current frame took

	u32 target_FPS;		// current FPS
	u32 frame_count;	// helps calculate FPS
} Time_State;


// ==================  New Version  ==================
typedef struct {

	f32 delta;				// time between frames
	f32 last;				// time of last frame
	f32 frame_time;			// time this frame took to calc (only frame calculation)
	u32 fps;				// current FPS
	f32 target_frame_delay;	// the targeted FPS in seconds x = 1/FSP

} time_info;
// ==================  New Version  ==================


typedef struct {
	int TI_year;
	int TI_month;
	int TI_day;
	int TI_hour;
	int TI_minute;
	int TI_sec;
	int TI_nSec;
} time_stamp;

void time_init(time_info* time_info, u32 target_FPS);
void time_update(time_info* time_info);
void time_calc_frame_time(time_info* time_info);
void time_limit_FPS(time_info* time_info);

PFF_API u64 Get_TimeStamp(void);
PFF_API float Calc_Time_Dif_in_Seconds(const u64 startTimeStamp, const u64 endTimeStamp);
PFF_API u64 Subtract_Seconds_From_TimeStamp(const u64 originalTimeStamp, const float secondsToSubtract);
