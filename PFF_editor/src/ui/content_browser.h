#pragma once

#include "editor_window.h"

namespace PFF {

	class content_browser : public editor_window {
	public:

		content_browser();

		void window() override;

	private:
		
		FORCEINLINE void show_directory_tree(const std::filesystem::path& path);

		std::filesystem::path		m_project_directory;
		std::filesystem::path		m_selected_directory;
	};

}