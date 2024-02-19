#pragma once

namespace PFF {

	class PFF_API timer {

	public:

		timer() {};
		~timer() {};

		//DELETE_COPY(timer);

		void start_measurement();
		void end_measurement(f32& work_time);

		void set_fps_settings(u32 target_fps);
		void limit_fps(const bool limit, u32& fps, f32& delta_time, f32& work_time, f32& sleep_time);

	private:

		std::chrono::time_point<std::chrono::steady_clock>  m_start_time{};
		std::chrono::time_point<std::chrono::steady_clock>  m_end_time{};
		f32 target_duration{};
		std::chrono::duration<f32> sleep_duration{};


		f32 m_last_frame_time = 0.f;

	};

}