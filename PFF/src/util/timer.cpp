
#include "util/pffpch.h"
#include <GLFW/glfw3.h>

#include "timer.h"

namespace PFF {

	void custom_sleep_for(f32 duration) {

		const int32 estimated_diviation = 10000;

		auto currentTime = std::chrono::steady_clock::now();
		auto target_time = currentTime + std::chrono::milliseconds(static_cast<long long>(duration));

		if ((duration - estimated_diviation) > 0)
			std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<f32>(duration - estimated_diviation)));

		// busy wait until target_time
		while (target_time >= std::chrono::steady_clock::now())
			;
	}

	void timer::set_fps_settings(u32 target_fps) {

		target_duration = static_cast<f32>(1.0 / target_fps);
	}

	void timer::start_measurement() {
		
		m_last_frame_time = static_cast<f32>(glfwGetTime());
	}

	void timer::end_measurement(f32& work_time) {

		work_time = static_cast<f32>(glfwGetTime()) - m_last_frame_time;
	}

	void timer::limit_fps(const bool limit, u32& fps, f32& delta_time, f32& work_time, f32& sleep_time) {

		PFF_PROFILE_FUNCTION();

		work_time = static_cast<f32>(glfwGetTime()) - m_last_frame_time;
		if (work_time < target_duration && limit) {

			PFF_PROFILE_SCOPE("sleep");
			sleep_time = (target_duration - work_time) * 1000;
			PFF::custom_sleep_for(sleep_time);
		} else
			sleep_time = 0;

		f32 time = static_cast<f32>(glfwGetTime());
		delta_time = time - m_last_frame_time;
		m_last_frame_time = time;
		fps = static_cast<u32>(1.0 / (work_time + (sleep_time * 0.001)) + 0.5); // Round to nearest integer
	}

}
