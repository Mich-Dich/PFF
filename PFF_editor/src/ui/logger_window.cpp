
#include <util/pch_editor.h>

#include "logger_window.h"

namespace PFF {

	logger_window::logger_window() { m_log_file_loc = logger::get_log_file_location(); }

	void logger_window::window() {

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::Begin("Log Display", nullptr, window_flags);

		// TODO: currently: open, read, close, the log file every frame			=> SHIT

		f32 time_open_file = 0;
		stopwatch stopwatch_open_file = stopwatch(&time_open_file, duration_precision::microseconds);
		std::ifstream file(m_log_file_loc);
		if (!file.is_open()) {

			LOG(Error, "Could not open file [" << m_log_file_loc << "]");
			ImGui::End();
			return;
		}
		stopwatch_open_file.stop();


		f32 time_read_file = 0;
		stopwatch stopwatch_read_file = stopwatch(&time_read_file, duration_precision::microseconds);
		std::string line;
		while (std::getline(file, line))
			UI::anci_text(line);
		stopwatch_read_file.stop();


		f32 time_close_file = 0;
		stopwatch stopwatch_close_file = stopwatch(&time_close_file, duration_precision::microseconds);
		file.close();
		stopwatch_close_file.stop();

		if (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.f);

		m_auto_scroll = (ImGui::GetScrollY() < ImGui::GetScrollMaxY());				// TODO: disable auto scroll when scrolling up && reingage when scrolling to bottom

		ImGui::End();


		LOG(Trace, "time_open_file : " << time_open_file);
		LOG(Trace, "time_read_file : " << time_read_file);
		LOG(Trace, "time_close_file  : " << time_close_file);
	}

}
