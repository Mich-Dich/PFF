
#include "util/pffpch.h"

#include "timer.h"

namespace PFF {

	void timer::start_measurement() {

		PFF_PROFILE_FUNCTION();
		m_start_time = std::chrono::high_resolution_clock::now();
	}

	void timer::end_measurement(u32& fps, f32& delta_time, f64& work_time, f64& sleep_time) {

		PFF_PROFILE_FUNCTION();
		// Simple FPS controller - needs work
		m_end_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<f64> elapsed_time = m_end_time - m_start_time;
		work_time = elapsed_time.count();
		delta_time = static_cast<f32>(work_time);
		sleep_time = 0;
		fps = static_cast<u32>(1 / (work_time + sleep_time));
	}

	void timer::set_fps_settings(u32 target_fps) {

		PFF_PROFILE_FUNCTION();
		target_duration = std::chrono::duration<f64>(1.0 / target_fps);
	}

	void timer::limit_fps(const bool limit,  u32& fps, f32& m_delta_time, f64& work_time, f64& sleep_time) {

		PFF_PROFILE_FUNCTION();
		m_end_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<f64> elapsed_time = m_end_time - m_start_time;
		work_time = elapsed_time.count();

		if (target_duration > elapsed_time && limit) {

			sleep_duration = (target_duration - elapsed_time);
			std::this_thread::sleep_for(sleep_duration);
			sleep_time = sleep_duration.count();

		} else
			sleep_time = 0;

		m_start_time = std::chrono::high_resolution_clock::now();
		m_delta_time = static_cast<f32>(work_time + sleep_time);
		fps = static_cast<u32>(1 / (work_time + sleep_time));
	}

}
