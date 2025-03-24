#pragma once

#include "editor_window.h"

namespace PFF {

	//struct text_section {
	//	ImVec4			color{};
	//	size_t			start_pos = 0;
	//	size_t			count = 0;
	//};
	
	class logger_window : public editor_window {
	public:

		logger_window();
		void window() override;

	private:
		std::filesystem::path		m_log_file_loc{};
		bool						m_auto_scroll = true;

		//std::vector<text_section>	m_sections{};
		//std::string					m_complete_file_content{};
	};

}
