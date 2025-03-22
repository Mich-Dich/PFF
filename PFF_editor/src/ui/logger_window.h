#pragma once

#include "editor_window.h"

namespace PFF {

	class logger_window : public editor_window {
	public:

		logger_window();
		void window() override;

	private:
		std::filesystem::path		m_log_file_loc{};
		bool						m_auto_scroll = true;
	};

}
