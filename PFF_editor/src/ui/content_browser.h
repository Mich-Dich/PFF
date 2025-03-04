#pragma once

#include "editor_window.h"

namespace PFF {

	class content_browser{
	public:

		content_browser();
		~content_browser();

		void window();

	private:

		void select_new_directory(const std::filesystem::path& path);
		void show_directory_tree(const std::filesystem::path& path);
		void show_search_result_for_current_folder(const std::filesystem::path& path, u32& item_index);
		void show_current_folder_content(const std::filesystem::path& path);

		void display_file(const std::filesystem::path file_path, int ID);

		std::filesystem::path				m_project_directory;
		std::filesystem::path				m_selected_directory;
		std::filesystem::path				m_partial_selected_directory;
		
		std::set<std::filesystem::path>		m_selected_items;							// Set to keep track of selected items
		std::filesystem::path				m_main_selected_item;

		ref<image>							m_folder_icon;
		ref<image>							m_folder_big_icon;
		ref<image>							m_folder_open_icon;
		ref<image>							m_world_icon;
		ref<image>							m_mesh_asset_icon;
		ImVec2								m_icon_size;
	};

}