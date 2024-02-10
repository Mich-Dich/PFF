#pragma once

namespace PFF {

	class PFF_API timer {

	public:

		timer() {};
		~timer() {};

		DELETE_COPY(timer);

		void start_measurement();
		void end_measurement(f64& work_time);

		void set_fps_settings(u32 target_fps);
		void limit_fps(const bool limit, u32& fps, f32& delta_time, f64& work_time, f64& sleep_time);

	private:

		std::chrono::time_point<std::chrono::steady_clock>  m_start_time{};
		std::chrono::time_point<std::chrono::steady_clock>  m_end_time{};
		std::chrono::duration<f64> target_duration{};
		std::chrono::duration<f64> sleep_duration{};
	};

}