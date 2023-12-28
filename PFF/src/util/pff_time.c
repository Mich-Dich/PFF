
#include <SDL.h>

#include "util/IOCore/logger.h"

#include "pff_time.h"


// ========================================================================================== PUBLIC FUNCS ==========================================================================================

//
void time_init(fps_time_info* time_info, u32 target_FPS) {

	if (target_FPS == 0)
		time_info->target_frame_delay = 0;
	else
		time_info->target_frame_delay = 1000.f / (f32)target_FPS;
	
	LOG_FUNC_END("")
}

// calculated the FPS for every loop
void time_update(fps_time_info* time_info) {

	f32 now = (f32)SDL_GetTicks();
	time_info->delta = (now - time_info->last) / 1000.f;
	time_info->fps = (u32)(1 / time_info->delta);
	time_info->last = now;
}

// to be called right after Frame is finished calculating
void time_calc_frame_time(fps_time_info* time_info) {

	time_info->frame_time = (f32)SDL_GetTicks() - time_info->last;
}

//
void time_limit_FPS(fps_time_info* time_info) {

	time_info->frame_time = (f32)SDL_GetTicks() - time_info->last;

	if (time_info->target_frame_delay > time_info->frame_time) {
		SDL_Delay((Uint32)(time_info->target_frame_delay - time_info->frame_time));
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
float Calc_Time_Dif_in_Seconds(const u64 startTimeStamp, const u64 endTimeStamp) {

	u64 timeDiff = endTimeStamp - startTimeStamp;
	return timeDiff * (1.0f / 1e7f);
}

//
u64 Subtract_Seconds_From_TimeStamp(const u64 originalTimeStamp, const float secondsToSubtract) {

	u64 intervalsToSubtract = (u64)(secondsToSubtract * 1e7);
	return originalTimeStamp - intervalsToSubtract;
}
