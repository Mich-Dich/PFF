#pragma once

#include "editor_window.h"

namespace PFF {

	class content_browser : public editor_window {
	public:

		content_browser();
		~content_browser();

		void window() override;

	private:
		

		FORCEINLINE void select_new_directory(const std::filesystem::path& path);
		FORCEINLINE void show_directory_tree(const std::filesystem::path& path);
		FORCEINLINE void show_current_folder_content(const std::filesystem::path& path);

		std::filesystem::path		m_project_directory;
		std::filesystem::path		m_selected_directory;
		std::filesystem::path		m_partial_selected_directory;

		ref<image>					m_folder_image;
		ref<image>					m_mesh_asset_image;
		ImVec2						m_icon_size;
	};

}