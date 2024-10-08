#pragma once

#include "editor_window.h"

namespace PFF {

	class content_browser{
	public:

		content_browser();
		~content_browser();

		void window();

	private:

		FORCEINLINE void select_new_directory(const std::filesystem::path& path);
		FORCEINLINE void show_directory_tree(const std::filesystem::path& path);
		FORCEINLINE void show_search_result_for_current_folder(const std::filesystem::path& path, u32& item_index);
		FORCEINLINE void show_current_folder_content(const std::filesystem::path& path);

		void display_file(std::filesystem::path file_path, u32 ID);

		std::filesystem::path		m_project_directory;
		std::filesystem::path		m_selected_directory;
		std::filesystem::path		m_partial_selected_directory;

		ref<image>					m_folder_image;
		ref<image>					m_world_image;
		ref<image>					m_mesh_asset_image;
		ImVec2						m_icon_size;
	};

}