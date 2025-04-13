#pragma once

#include "editor_window.h"

namespace PFF {

	struct selected_files {

		std::filesystem::path				main_item;
		std::set<std::filesystem::path>		item_set;

		void reset() {
			
			item_set.clear();
			main_item = std::filesystem::path();
		}

		inline bool empty() { return main_item.empty() && item_set.empty(); }
	};

	struct deletion_consequenses {
		std::set<std::string>				effected_assets{};
		u32 								number_of_files = 0;
		u32 								number_of_directoryies = 0;

		void reset() {
			effected_assets.clear();
			number_of_files = 0;
			number_of_directoryies = 0;
		}
	};

	class content_browser{
	public:

		content_browser();
		~content_browser();

		void window();

		
	private:

		void call_to_delete_file(const std::filesystem::path file_path);
		void select_new_directory(const std::filesystem::path& path);
		void show_directory_tree(const std::filesystem::path& path);
		void show_search_result_for_current_folder(const std::filesystem::path& path, u32& item_index);
		void show_current_folder_content(const std::filesystem::path& path);
		
		void display_file(const std::filesystem::path file_path, int ID);
		void handel_deletion_action(const std::filesystem::path file_path);
		
		std::filesystem::path					m_project_directory;
		std::filesystem::path					m_selected_directory;
		std::filesystem::path					m_partial_selected_directory;
		
#if 1
		selected_files							m_selected_items{};
#else
		// std::set<std::filesystem::path>			m_selected_items;							// Set to keep track of selected items
		// std::filesystem::path					m_main_selected_item;
#endif
		bool   									m_block_mouse_input = false;
		bool   									logged_warning_for_current_folder = false;


		deletion_consequenses					m_deletion_consequenses{};
		bool									m_deletion_popup = false;
		std::filesystem::path					m_path_to_delete{};
		// std::vector<std::string>				m_still_used_files{};

		ref<image>								m_folder_icon;
		ref<image>								m_folder_big_icon;
		ref<image>								m_folder_open_icon;
		ref<image>								m_world_icon;
		ref<image>								m_warning_icon;
		ref<image>								m_mesh_asset_icon;
		ImVec2									m_icon_size;
	};

}