
#include "util/pffpch.h"

#include "timer.h"

namespace PFF {

	void timer::set_fps_settings(u32 target_fps) {

		target_duration = std::chrono::duration<f64>(1.0 / target_fps);
	}

	void timer::start_measurement() {

		PFF_PROFILE_FUNCTION();
		m_start_time = std::chrono::steady_clock::now();
	}

	void timer::end_measurement(f64& work_time) {

		PFF_PROFILE_FUNCTION();

		// Simple FPS controller - needs work
		auto end_timepoint = std::chrono::steady_clock::now();
		auto high_res_start = std::chrono::duration<double, std::micro>{ m_start_time.time_since_epoch() };
		auto elapsed_time = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_start_time).time_since_epoch();

		work_time = elapsed_time.count() * 0.000001;
	}

	void timer::limit_fps(const bool limit,  u32& fps, f32& delta_time, f64& work_time, f64& sleep_time) {

		PFF_PROFILE_FUNCTION();
		
		end_measurement(work_time);
		if (target_duration.count() > work_time && limit) {

			sleep_duration = static_cast<std::chrono::duration<f64>>(target_duration.count() - work_time);
			std::this_thread::sleep_for(sleep_duration);
			sleep_time = sleep_duration.count();

		} else
			sleep_time = 0;

		start_measurement();
		delta_time = static_cast<f32>(work_time + sleep_time);
		fps = static_cast<u32>(1 / (work_time + sleep_time));
	}

}
