
#include <util/pch_editor.h>

#include "logger_window.h"

namespace PFF {

	logger_window::logger_window() { m_log_file_loc = logger::get_log_file_location(); }

	void logger_window::window() {

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::Begin("Log Display", nullptr, window_flags);


		std::ifstream file(m_log_file_loc);
		if (!file.is_open()) {

			LOG(Error, "Could not open file [" << m_log_file_loc << "]");
			ImGui::End();
			return;
		}

		std::string line;
		while (std::getline(file, line))
			UI::anci_text(line);

		file.close();

		if (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.f);

		m_auto_scroll = (ImGui::GetScrollY() < ImGui::GetScrollMaxY());				// TODO: disable auto scroll when scrolling up && reingage when scrolling to bottom

		ImGui::End();
	}

}
