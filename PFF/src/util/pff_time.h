#pragma once

#include "util/types.h"

#include "dll_interface.h"


typedef struct {

	f32 delta;				// time between frames
	f32 last;				// time of last frame
	f32 frame_time;			// time this frame took to calc (only frame calculation)
	u32 fps;				// current FPS
	f32 target_frame_delay;	// the targeted FPS in seconds x = 1/FSP

} fps_time_info;


typedef struct {
	int TI_year;
	int TI_month;
	int TI_day;
	int TI_hour;
	int TI_minute;
	int TI_sec;
	int TI_nSec;
} time_stamp;


void time_init(fps_time_info* time_info, u32 target_FPS);
void time_update(fps_time_info* time_info);
void time_calc_frame_time(fps_time_info* time_info);
void time_limit_FPS(fps_time_info* time_info);

PFF_API u64 Get_TimeStamp(void);
PFF_API float Calc_Time_Dif_in_Seconds(const u64 startTimeStamp, const u64 endTimeStamp);
PFF_API u64 Subtract_Seconds_From_TimeStamp(const u64 originalTimeStamp, const float secondsToSubtract);
