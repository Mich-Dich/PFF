#include "core/pch.h"
#include "pff_time.h"

#include "core/config.h"
#include "core/input.h"
#include "core/types.h"
#include "core/logger.h"
#include "core/global.h"

//
void time_init(u32 frame_rate) {

	CL_LOG_FUNC_START("")
	global.time.frame_rate = frame_rate;
	global.time.frame_delay = 1000.f / frame_rate;
	CL_LOG_FUNC_END("")
}

//
void time_update(void) {

	global.time.now = (f32)SDL_GetTicks();
	global.time.delta = (global.time.now - global.time.last) / 1000.f;
	global.time.last = global.time.now;
	++global.time.frame_count;

	// calc FPS every second
	if (global.time.now - global.time.frame_last >= 1000.f) {
		global.time.frame_rate = global.time.frame_count;
		global.time.frame_count = 0;
		global.time.frame_last = global.time.now;
	}
}

//
void time_update_late(void) {

	global.time.frame_time = (f32)SDL_GetTicks() - global.time.now;

	if (global.time.frame_delay > global.time.frame_time) {
		SDL_Delay((Uint32)(global.time.frame_delay - global.time.frame_time));
	}
}


//
u64 Get_TimeStamp(void) {

	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);

	ULARGE_INTEGER ull;
	ull.LowPart = fileTime.dwLowDateTime;
	ull.HighPart = fileTime.dwHighDateTime;
	return ull.QuadPart;
}

//
float Calc_Time_Dif_in_Seconds(u64 startTimeStamp, u64 endTimeStamp) {

	// Convert 100-nanosecond intervals to seconds (1 second = 10^7 100-nanosecond intervals)
	const float secondsPerInterval = 1.0 / 1e7;
	u64 timeDiff = endTimeStamp - startTimeStamp;
	return timeDiff * secondsPerInterval;
}

//
u64 Subtract_Seconds_From_TimeStamp(u64 originalTimeStamp, float secondsToSubtract) {

	u64 intervalsToSubtract = (u64)(secondsToSubtract * 1e7);
	return originalTimeStamp - intervalsToSubtract;
}