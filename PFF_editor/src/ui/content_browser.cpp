
#include "util/pch_editor.h"

#include <imgui_internal.h>
#include <util/ui/pannel_collection.h>
#include <engine/resource_management/headers.h>
#include <engine/resource_management/mesh_headers.h>
#include <engine/resource_management/static_mesh_asset_manager.h>

#include "ui/import/mesh_import_window.h"
#include "ui/import/texture_import_window.h"
#include "util/import_dialog.h"
#include "PFF_editor.h"
#include "editor_layer.h"

#include "content_browser.h"


#define ITEM_DRAD_DRAP_SOURCE_NAME_LENGTH			9



namespace PFF {

	static ImGuiTreeNodeFlags	base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	static ImU32				background_color = IM_COL32(50, 50, 50, 255);
	static std::string			search_query;
	const ImVec2				folder_open_icon_size(18, 14);																		// Calculate the icon size for the tree (make it smaller than regular icons)
	const ImVec2				folder_closed_icon_size(18, 17);																	// Calculate the icon size for the tree (make it smaller than regular icons)
	ImGuiWindow*				folder_display_window{};

	enum file_curruption_source {
		unknown = 0,
		empty_file,
		header_incorrect,
	};

	const std::vector<std::pair<std::string, std::string>> posible_import_tile_types = {
		{"All supported file types", "*.gltf;*.glb;*.png"},
		{"glTF 2.0 file", "*.gltf;*.glb"},												// for meshes
		{"PNG", "*.png"},																// for images
	};


	int hash_path(const std::filesystem::path& path) { return static_cast<int>(std::hash<std::string>()(path.string())); }


	static void drop_target_to_move_file(const std::filesystem::path folder_path) {

		if (ImGui::BeginDragDropTarget()) {

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FILE")) {

				const std::filesystem::path file_path = (const char*)payload->Data;
				try {

					std::filesystem::path destination = folder_path / file_path.filename();
					std::filesystem::rename(file_path, destination);
					LOG(Info, "File moved successfully!");

				} catch (const std::filesystem::filesystem_error& e)
					LOG(Error, "Error: " << e.what());

			}
			ImGui::EndDragDropTarget();
		}
	}


	content_browser::content_browser() {

		m_project_directory = PFF_editor::get().get_project_path();
		select_new_directory(m_project_directory / CONTENT_DIR);

		// load folder image
		auto* editor_layer = PFF_editor::get().get_editor_layer();
		m_folder_icon =			editor_layer->get_folder_icon();
		m_folder_big_icon =		editor_layer->get_folder_big_icon();
		m_folder_open_icon =	editor_layer->get_folder_icon();
		m_world_icon =			editor_layer->get_world_icon();
		m_warning_icon =		editor_layer->get_warning_icon();
		m_mesh_asset_icon =		editor_layer->get_mesh_asset_icon();

		m_icon_size = { 60, 60 };
	}


	content_browser::~content_browser() {

		m_folder_icon.reset();
		m_folder_big_icon.reset();
		m_folder_open_icon.reset();
		m_world_icon.reset();
		m_mesh_asset_icon.reset();
	}


	void content_browser::select_new_directory(const std::filesystem::path& path) {

		m_selected_directory = path;
		m_partial_selected_directory = util::extract_path_from_project_folder(path);
		logged_warning_for_current_folder = false;
	}


	void content_browser::show_directory_tree(const std::filesystem::path& path) {

		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (!entry.is_directory())
				continue;

			bool has_sub_folders = false;
			for (const auto& sub_entry : std::filesystem::directory_iterator(entry.path())) {
				if (!sub_entry.is_directory())
					continue;

				has_sub_folders = true;
				break;
			}

			const std::string path_string = entry.path().filename().string();

			// --------------------------------------------- THIS CODE HERE ---------------------------------------------
			if (has_sub_folders) {

				ImGui::PushID(hash_path(entry.path()));
				bool buffer = ImGui::TreeNodeEx(("##" + path_string).c_str(), base_flags);				// Create tree node without the label
				ImGui::PopID();

				ImGui::SameLine(0, 2);																						// Place everything on same line
				ImGui::BeginGroup();																						// Start group to align items horizontally

				if (buffer)
					ImGui::Image(m_folder_open_icon->get_descriptor_set(), folder_open_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::get_main_color_ref());
				else
					ImGui::Image(m_folder_icon->get_descriptor_set(), folder_closed_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::get_main_color_ref());

				ImGui::SameLine(0, 2);
				ImGui::Text("%s", path_string.c_str());												// Draw folder name text
				ImGui::EndGroup();

				if (ImGui::IsItemClicked())
					select_new_directory(entry.path());

				drop_target_to_move_file(entry.path());

				if (buffer) {
					show_directory_tree(entry.path());
					ImGui::TreePop();
				}
			} else {

				ImGui::PushID(hash_path(entry.path()));
				ImGui::TreeNodeEx("##", base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
				ImGui::PopID();

				ImGui::SameLine(0, 2);
				ImGui::BeginGroup();																						// Similar modification for leaf nodes
				ImGui::Image(m_folder_icon->get_descriptor_set(), folder_closed_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::get_main_color_ref());
				ImGui::SameLine(0, 2);
				ImGui::Text("%s", path_string.c_str());
				ImGui::EndGroup();

				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					select_new_directory(entry.path());

				drop_target_to_move_file(entry.path());
			}

			// --------------------------------------------- THIS CODE HERE ---------------------------------------------
		}

	}

	// Function to handle dropping files into the current directory
	void handle_drop(const std::filesystem::path& target_directory) {

		// Implement file move logic here
		LOG(Info, "NOT IMPLEMENTED YET");
	}

	//static std::string search_query;
	void content_browser::show_search_result_for_current_folder(const std::filesystem::path& path, u32& item_index) {

		const float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (entry.is_directory())
				show_search_result_for_current_folder(entry.path(), item_index);			// look into any subfolder
			else {

				if (entry.path().filename().string().find(search_query) == std::string::npos)
					continue;

				display_file(entry.path(), item_index++);

				// handle item wrapping
				const float next_item_x2 = ImGui::GetItemRectMax().x + ImGui::GetStyle().ItemSpacing.x + m_icon_size.x; // Expected position if next item was on the same line
				if (next_item_x2 < window_visible_x2)
					ImGui::SameLine();
			}
		}
	}


	void content_browser::show_current_folder_content(const std::filesystem::path& path) {

		const ImGuiStyle& style = ImGui::GetStyle();
		const float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		u32 count = 0;

		for (const auto& entry : std::filesystem::directory_iterator(path)) {											// display directorys first

			if (!entry.is_directory())
				continue;

			// Begin a new group for each item
			const int current_ID = hash_path(entry.path());
			const std::string item_name = entry.path().filename().string();
			const std::string wrapped_text = UI::wrap_text_at_underscore(item_name, m_icon_size.x);
			const std::string popup_name = "dir_context_menu_" + item_name;
			const ImVec4& color = (m_selected_items.item_set.find(entry.path()) != m_selected_items.item_set.end()) ? UI::get_action_color_00_active_ref() : UI::get_action_color_gray_active_ref();
			ImGui::BeginGroup();
			{
				ImGui::PushID(current_ID);
				ImGui::Image(m_folder_big_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color);
				ImVec2 text_size = ImGui::CalcTextSize(wrapped_text.c_str());
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((m_icon_size.x - text_size.x) * 0.5f, 0));
				ImGui::TextWrapped("%s", wrapped_text.c_str());
				ImGui::PopID();
			}
			ImGui::EndGroup();

			const auto dir_mouse_interation = UI::get_mouse_interation_on_item(m_block_mouse_input);
			switch (dir_mouse_interation) {
				//case UI::mouse_interation::none: 					LOG(Trace, "none"); break;
				//case UI::mouse_interation::hovered: 				LOG(Trace, "hovered"); break;
				case UI::mouse_interation::left_clicked: 			LOG(Trace, "left_clicked"); break;
				//case UI::mouse_interation::left_double_clicked: 	LOG(Trace, "left_double_clicked"); break;
				case UI::mouse_interation::left_pressed:			break;
				case UI::mouse_interation::left_released: 			LOG(Trace, "left_released"); break;
				case UI::mouse_interation::right_clicked:			ImGui::OpenPopup(popup_name.c_str()); break;
				case UI::mouse_interation::right_double_clicked: 	LOG(Trace, "right_double_clicked"); break;
				case UI::mouse_interation::right_pressed: 			LOG(Trace, "right_pressed"); break;
				case UI::mouse_interation::right_released: 			LOG(Trace, "right_released"); break;
				case UI::mouse_interation::middle_clicked: 			LOG(Trace, "middle_clicked"); break;
				case UI::mouse_interation::middle_double_clicked: 	LOG(Trace, "middle_double_clicked"); break;
				case UI::mouse_interation::middle_pressed: 			LOG(Trace, "middle_pressed"); break;
				case UI::mouse_interation::middle_release: 			LOG(Trace, "middle_release"); break;
				case UI::mouse_interation::dragged: 				LOG(Trace, "dragged"); break;
				case UI::mouse_interation::focused: 				LOG(Trace, "focused"); break;
				case UI::mouse_interation::active: 					LOG(Trace, "active"); break;
				case UI::mouse_interation::deactivated: 			LOG(Trace, "deactivated"); break;
				case UI::mouse_interation::deactivated_after_edit: 	LOG(Trace, "deactivated_after_edit"); break;

				case UI::mouse_interation::left_double_clicked:
					LOG(Trace, "Set TreeNode to open ID[" << current_ID << "]");
					m_selected_items.reset();
					ImGui::TreeNodeSetOpen(folder_display_window->GetID(current_ID), true);
					m_block_mouse_input = true;
					select_new_directory(entry.path());
					return;

				default: break;
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {

				char shortened_item_name[ITEM_DRAD_DRAP_SOURCE_NAME_LENGTH + 4];
				if (item_name.length() -1 > ITEM_DRAD_DRAP_SOURCE_NAME_LENGTH) {

					const auto buffer = item_name.substr(0, ITEM_DRAD_DRAP_SOURCE_NAME_LENGTH);
					snprintf(shortened_item_name, sizeof(shortened_item_name), "%s...", buffer.c_str());
				}
				else
					snprintf(shortened_item_name, sizeof(shortened_item_name), "%s", item_name.c_str());

				const std::string path_string = entry.path().string();
				ImGui::SetDragDropPayload("PROJECT_CONTENT_FOLDER", path_string.c_str(), path_string.length() + 1);
				ImGui::Image(m_folder_big_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::get_action_color_gray_active_ref());
				ImGui::Text("%s", shortened_item_name);
				ImGui::EndDragDropSource();
			}

			UI::adjust_popup_to_window_bounds(ImVec2(20, 50));
			if (ImGui::BeginPopupContextItem(popup_name.c_str())) {

				if (dir_mouse_interation == UI::mouse_interation::none && !UI::is_holvering_window())
					ImGui::CloseCurrentPopup();

				// Set the adjusted position

				if (ImGui::MenuItem("Rename"))
					LOG(Info, "NOT IMPLEMENTED YET");

				if (ImGui::MenuItem("Delete")) {								// open popup to display consequences of deleting file and ask again

					m_deletion_popup = true;
					m_path_to_delete = entry.path();
				}

				ImGui::EndPopup();
			}

			// Handle dropping files into the current directory
			drop_target_to_move_file(entry.path());

			const float next_item_x2 = ImGui::GetItemRectMax().x + style.ItemSpacing.x + m_icon_size.x;		// Expected position if next item was on the same line
			if (next_item_x2 < window_visible_x2)															// handle item wrapping
				ImGui::SameLine();
		}

		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (entry.is_directory())
				continue;

			const std::filesystem::path current_path = entry.path();
			const int current_ID = hash_path(entry.path());
			display_file(current_path, current_ID);
			const float next_item_x2 = ImGui::GetItemRectMax().x + style.ItemSpacing.x + m_icon_size.x;		// Expected position if next item was on the same line
			if (next_item_x2 < window_visible_x2)															// handle item wrapping
				ImGui::SameLine();

		}

		// Handle dropping files into the current directory
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FOLDER")) {

				// Handle folder drop
			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FILE")) {
				std::string file_path(static_cast<const char*>(payload->Data));
				LOG(Error, "file_path: " << file_path);
				handle_drop(path / std::filesystem::path(file_path).filename());
			}
			ImGui::EndDragDropTarget();
		}

		logged_warning_for_current_folder = true;
	}


	bool try_to_deserialize_file_header(const std::filesystem::path file_path, const bool log_messages, file_curruption_source& file_curruption_source, asset_file_header& loc_asset_file_header) {

		bool file_currupted = false;
		file_curruption_source = file_curruption_source::unknown;

		std::error_code loc_error_code;
		if (!std::filesystem::file_size(file_path, loc_error_code)) {

			file_curruption_source = file_curruption_source::empty_file;
			file_currupted = true;

			std::string faulty_file_name = file_path.filename().generic_string();
			if (log_messages)
				LOG(Warn, "file [" << faulty_file_name << "] is an empty file, will be marked as currupted");
		}


		if (file_path.extension() == ".pffasset") {

			serializer::binary(file_path, "PFF_asset_file", serializer::option::load_from_file)
				.entry(loc_asset_file_header);

		} else if (file_path.extension() == ".pffworld") {

			serializer::yaml(file_path, "map_data", serializer::option::load_from_file)
				.sub_section("file_header", [&](serializer::yaml& header_section) {

					header_section.entry(KEY_VALUE(loc_asset_file_header.file_version))
					.entry(KEY_VALUE(loc_asset_file_header.type))
					.entry(KEY_VALUE(loc_asset_file_header.timestamp));
			});
		}

		return file_currupted;
	}


	void content_browser::display_file(const std::filesystem::path file_path, int ID) {

		bool file_currupted = false;
		file_curruption_source loc_file_curruption_source = file_curruption_source::unknown;
		asset_file_header loc_asset_file_header;
		file_currupted = try_to_deserialize_file_header(file_path, !logged_warning_for_current_folder, loc_file_curruption_source, loc_asset_file_header);

		// Begin a new group for each item
		const ImVec4& color = (file_path == m_selected_items.main_item) ? UI::get_action_color_00_active_ref() : (m_selected_items.item_set.find(file_path) != m_selected_items.item_set.end()) ? UI::get_action_color_00_faded_ref() : ImVec4(1);
		const std::string item_name = file_path.filename().replace_extension("").string();
		ImGui::PushID(ID);
		ImGui::BeginGroup();
		{

			const ImVec2 item_pos = ImGui::GetCursorScreenPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(item_pos, item_pos + m_icon_size, background_color, ImGui::GetStyle().FrameRounding);

			if (file_currupted) {

				ImGui::Image(m_warning_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), (file_path == m_selected_items.main_item) ? ImVec4(.9f, .5f, 0.f, 1.f) : (m_selected_items.item_set.find(file_path) != m_selected_items.item_set.end()) ? ImVec4(.8f, .4f, 0.f, 1.f) : ImVec4(1.f, .6f, 0.f, 1.f));

			} else {

				switch (loc_asset_file_header.type) {
					case file_type::mesh:
						ImGui::Image(m_mesh_asset_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color);
						break;

					case file_type::world:
						ImGui::Image(m_world_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color);
						break;

					default:
						file_currupted = true;
						loc_file_curruption_source = file_curruption_source::header_incorrect;
						std::string faulty_file_name = file_path.filename().generic_string();
						if (!logged_warning_for_current_folder)
							LOG(Warn, "file [" << faulty_file_name << "] cound not be identified, detected asset header type [" << asset_header_file_type_to_str(loc_asset_file_header.type) << "]");
						ImGui::Image(m_warning_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), (file_path == m_selected_items.main_item) ? ImVec4(.9f, .5f, 0.f, 1.f) : (m_selected_items.item_set.find(file_path) != m_selected_items.item_set.end()) ? ImVec4(.8f, .4f, 0.f, 1.f) : ImVec4(1.f, .6f, 0.f, 1.f));
						break;
				}
			}

			const std::string wrapped_text = UI::wrap_text_at_underscore(item_name, m_icon_size.x);
			ImGui::TextWrapped("%s", wrapped_text.c_str());

		}
		ImGui::EndGroup();
		ImGui::PopID();
		auto item_mouse_interation = UI::get_mouse_interation_on_item(m_block_mouse_input);

		// Handle drag source for files
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {

			// LOG(Trace, "m_selected_items count: " << m_selected_items.size());
			if (m_selected_items.item_set.empty()) {

				char shortened_item_name[ITEM_DRAD_DRAP_SOURCE_NAME_LENGTH + 4];
				if (item_name.length() -1 > ITEM_DRAD_DRAP_SOURCE_NAME_LENGTH) {

					const auto buffer = item_name.substr(0, ITEM_DRAD_DRAP_SOURCE_NAME_LENGTH);
					snprintf(shortened_item_name, sizeof(shortened_item_name), "%s...", buffer.c_str());
				}
				else
					snprintf(shortened_item_name, sizeof(shortened_item_name), "%s", item_name.c_str());

				const std::string path_string = file_path.string();
				ImGui::SetDragDropPayload("PROJECT_CONTENT_FILE", path_string.c_str(), path_string.length() + 1);

				switch (loc_asset_file_header.type) {
					case file_type::mesh:		ImGui::Image(m_mesh_asset_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color); break;
					case file_type::world:		ImGui::Image(m_world_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color); break;
					default:					ImGui::Image(m_warning_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), (file_path == m_selected_items.main_item) ? ImVec4(.9f, .5f, 0.f, 1.f) : (m_selected_items.item_set.find(file_path) != m_selected_items.item_set.end()) ? ImVec4(.8f, .4f, 0.f, 1.f) : ImVec4(1.f, .6f, 0.f, 1.f)); break;
				}

				ImGui::Text("%s", shortened_item_name);
				ImGui::EndDragDropSource();

			} else {

				selected_files* payload_data = &m_selected_items;
				ImGui::SetDragDropPayload("PROJECT_CONTENT_FILE_MULTI", &payload_data, sizeof(payload_data));
				ImGui::Image(m_mesh_asset_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::get_action_color_gray_active_ref());
				ImGui::Text("[%ld] files", m_selected_items.item_set.size() + (m_selected_items.main_item.empty() ? 0 : 1));
				ImGui::EndDragDropSource();

			}
		}


		ImVec2 expected_size = ImVec2(20, 50);						// Adjust based on content
		if (file_currupted)
			switch (loc_file_curruption_source) {					// aprocimate size by corruption type
				default:
				case file_curruption_source::unknown:				expected_size = ImVec2(280, 250); break;	// should display everything to help user
				case file_curruption_source::header_incorrect:		expected_size = ImVec2(280, 250); break;	// should display header
				case file_curruption_source::empty_file:			expected_size = ImVec2(180, 150); break;	// dosnt need to display anything other than size
			}
		UI::adjust_popup_to_window_bounds(expected_size);

		std::string popup_name = "item_context_menu_" + item_name;
		if (ImGui::BeginPopupContextItem(popup_name.c_str())) {

			// Set the adjusted position

			if (ImGui::MenuItem("Rename"))
				LOG(Info, "NOT IMPLEMENTED YET");

			if (ImGui::MenuItem("Delete")) {								// open popup to display consequences of deleting file and ask again
	
				m_deletion_popup = true;
				m_block_mouse_input = true;
				item_mouse_interation = UI::mouse_interation::none;
				if (m_selected_items.main_item.empty() && m_selected_items.item_set.empty())
					m_path_to_delete = file_path;
			}

			if (file_currupted) {											// display everything we know about the file to help user find error

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				ImGui::Image(m_warning_icon->get_descriptor_set(), ImVec2(15), ImVec2(0, 0), ImVec2(1, 1), ImVec4(.9f, .5f, 0.f, 1.f));
				ImGui::SameLine();
				ImGui::Text("file is corrupted:");

				switch (loc_file_curruption_source) {
					default:
					case file_curruption_source::unknown:				ImGui::Text("Unknown reason for detecting corrupted file"); break;
					case file_curruption_source::header_incorrect:		ImGui::Text("File corrupted because deserialized header type is incorrect"); break;
					case file_curruption_source::empty_file:			ImGui::Text("File is empty and can't be used by game engine"); break;
				}

				ImGui::Text("general data");

				UI::begin_table("currupted_file_data_table", false);
				std::error_code error_code;
				const std::string_view file_size_in_MB = util::to_string((f32)std::filesystem::file_size(file_path, error_code) / 1024) + " MB";
				UI::table_row("file size", file_size_in_MB);
				ImGui::EndTable();

				if (loc_file_curruption_source != file_curruption_source::empty_file) {

					ImGui::Text("header data");

					UI::begin_table("currupted_file_data_table", false);
					UI::table_row("version", loc_asset_file_header.file_version.to_str() );
					UI::table_row("file type", asset_header_file_type_to_str(loc_asset_file_header.type) );
					UI::table_row("timestamp", loc_asset_file_header.timestamp.to_str() );
					ImGui::EndTable();
				}

			}

			if (item_mouse_interation == UI::mouse_interation::none && !UI::is_holvering_window())
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		switch (item_mouse_interation) {
			case UI::mouse_interation::left_double_clicked:
				LOG(Info, "NOT IMPLEMENTED YET => should opening coresponding editor window");
				break;

			case UI::mouse_interation::right_clicked: {

				ImGui::OpenPopup(popup_name.c_str());
			} break;

			case UI::mouse_interation::left_released:

				if (ImGui::GetIO().KeyShift) {
					
					if (!m_selected_items.main_item.empty()) {										// If main item selected -> perform range selection.

						std::vector<std::filesystem::path> files_in_dir;
						for (const auto& entry : std::filesystem::directory_iterator(m_selected_directory))
							files_in_dir.push_back(entry.path());

							auto it_main = std::find(files_in_dir.begin(), files_in_dir.end(), m_selected_items.main_item);
						auto it_clicked = std::find(files_in_dir.begin(), files_in_dir.end(), file_path);
						if (it_main != files_in_dir.end() && it_clicked != files_in_dir.end()) {

							auto start = std::min(it_main, it_clicked);
							auto end = std::max(it_main, it_clicked);
							m_selected_items.item_set.clear();
							for (auto it = start; it != std::next(end); ++it)
								m_selected_items.item_set.insert(*it);
						}

					} else
						m_selected_items.main_item = file_path;										// If main item not selected -> simply mark clicked item as main selection.
						
				} else if (ImGui::GetIO().KeyCtrl) {

					
					if (!m_selected_items.main_item.empty())
						m_selected_items.item_set.insert(m_selected_items.main_item);
					m_selected_items.main_item = file_path;

					// if (m_selected_items.item_set.find(file_path) == m_selected_items.item_set.end())				// If Shift is held, select the item
					// 	m_selected_items.item_set.insert(file_path);
					// else
					// 	m_selected_items.item_set.erase(file_path);

				} else {

					m_selected_items.item_set.clear();
					m_selected_items.main_item = file_path;
				}
				break;

			default: break;
		}

	}


	bool is_file_used(const std::filesystem::path file_path) {

		bool file_currupted = false;
		file_curruption_source loc_file_curruption_source = file_curruption_source::unknown;
		asset_file_header loc_asset_file_header;
		file_currupted = try_to_deserialize_file_header(file_path, false, loc_file_curruption_source, loc_asset_file_header);

		switch (loc_asset_file_header.type) {

			case file_type::world: 				return false;
			case file_type::map: 				return false;
			case file_type::audio: 				return false;
			case file_type::mesh: {
				
				// only checks for active use (in current world)	TODO: add a check that scans user code base for definitive anser
				bool in_use = static_mesh_asset_manager::is_asset_in_active_use(file_path);
				return in_use;
			}
			case file_type::texture: 			return false;
			case file_type::material: 			return false;
			case file_type::material_instance: 	return false;
			default:
			case file_type::none: 	return false;
		}

	}


	void ref_check_files_and_sub_directory(std::filesystem::path file_path, deletion_consequenses& deletion_consequenses) {

		if (!std::filesystem::is_directory(file_path))			// catch any missuses
			return;

		for (const auto path : std::filesystem::directory_iterator(file_path)) {

			if (std::filesystem::is_directory(path)) {

				deletion_consequenses.number_of_directoryies++;
				ref_check_files_and_sub_directory(path.path(), deletion_consequenses);
			} else {

				deletion_consequenses.number_of_files++;
				if (is_file_used(path.path()))
					deletion_consequenses.effected_assets.insert(path.path().filename().replace_extension("").string());
			}
		}
	}


	void get_deletion_consequenses(const std::filesystem::path file_path, deletion_consequenses& deletion_consequenses) {
		
		if (std::filesystem::is_directory(file_path)) {

			deletion_consequenses.number_of_directoryies++;
			ref_check_files_and_sub_directory(file_path, deletion_consequenses);
		
		} else {

			deletion_consequenses.number_of_files++;
			if (is_file_used(file_path))
				deletion_consequenses.effected_assets.insert(file_path.filename().replace_extension("").string());
		}
	}


	void content_browser::handel_deletion_action(const std::filesystem::path file_path) {

		VALIDATE(std::filesystem::exists(file_path), return, "deleting file [" << file_path << "]", "provided file path does not exist [" << file_path << "]");

		// TODO: remove the file from any asset managers (e.g. static_mesh_asset_manager)
		LOG(Warn, "Not completely implemented yet, TODO: remove the file from any asset managers (e.g. static_mesh_asset_manager)");


		std::error_code error_code{};
		if (std::filesystem::is_directory(file_path)) {
			
			VALIDATE(std::filesystem::remove(file_path, error_code), return, "deleting directory [" << file_path.filename().string() << "]", "FAILED to delete file from content folder: [" << file_path.generic_string() << "] error: " << error_code);
		} else {

			VALIDATE(std::filesystem::remove_all(file_path, error_code), return, "deleting asset [" << file_path.filename().string() << "]", "FAILED to delete file from content folder: [" << file_path.generic_string() << "] error: " << error_code);
		}
	}


	void content_browser::window() {

		//if (!show_window)
		//	return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::Begin("Content Browser", nullptr, window_flags);

		UI::custom_frame_NEW(350, true, ImGui::ColorConvertFloat4ToU32(UI::get_default_gray_1_ref()), [&]() {

			folder_display_window = ImGui::GetCurrentWindow();
			bool buffer = ImGui::TreeNodeEx(CONTENT_DIR, base_flags | ImGuiTreeNodeFlags_DefaultOpen);
			if (ImGui::IsItemClicked())
				select_new_directory(m_project_directory / CONTENT_DIR);

			if (buffer) {

				show_directory_tree(m_project_directory / CONTENT_DIR);
				ImGui::TreePop();
			}

			}, [&]() {

				if (m_block_mouse_input && ImGui::IsMouseReleased(ImGuiMouseButton_Left))			// block input when double clicking on a folder to avoid selecting new item at same location when switching dirs
					m_block_mouse_input = false;

				if (ImGui::Button(" import ##content_browser_import")) {

					std::filesystem::path source_path = util::file_dialog("Import asset", posible_import_tile_types);
					
					if (source_path.extension() == ".gltf" || source_path.extension() == ".glb")
						PFF_editor::get().get_editor_layer()->add_window<mesh_import_window>(source_path, m_selected_directory);
					
					else if (source_path.extension() == ".png")
						PFF_editor::get().get_editor_layer()->add_window<texture_import_window>(source_path, m_selected_directory);
						// LOG(Trace, "Tryed to import a PNG => not implemented yet")
					
					else
						LOG(Trace, "Tryed to import unsupported file type")										// TODO: add a notification system to main window
				}

				ImGui::SameLine();
				UI::shift_cursor_pos(30, 0);

				ImVec2 input_text_size(180, 0); // Set width to 150 pixels, height to default
				ImGui::SetNextItemWidth(math::clamp<f32>(input_text_size.x, 30, ImGui::GetContentRegionAvail().x));
				UI::serach_input("##serach_in_world_viewport_details_panel", search_query);

				ImGui::SameLine();
				UI::shift_cursor_pos(10, 0);
				std::filesystem::path current_path;
				for (const auto& part : m_partial_selected_directory) {

					current_path /= part;
					const std::string part_string = part.string();
					if (UI::gray_button(part_string.c_str())) {

						select_new_directory(m_project_directory / current_path);
						break;
					}
					drop_target_to_move_file(m_project_directory / current_path);

					if (part != m_partial_selected_directory.filename()) {

						ImGui::SameLine();
						ImGui::Text("/");
						ImGui::SameLine();
					}
				}

				if (UI::mouse_interation::right_clicked == UI::get_mouse_interation_on_window()) {

					LOG(Trace, "Open popup to add new content");
					ImGui::OpenPopup("##content_browser_current_dir_popup");
				}

				UI::adjust_popup_to_window_bounds(ImVec2(200, 200));
				if (ImGui::BeginPopup("##content_browser_current_dir_popup")) {


					if (ImGui::Button("create folder")) {

						LOG(Info, "Not implemented yet");
					}

					ImGui::Separator();
					ImGui::Text("Create New asset");

					if (ImGui::BeginMenu("world")) {

						if (ImGui::Button("Create empty world")) {

							LOG(Info, "Not implemented yet");
						}

						ImGui::EndMenu();
					}

					if (ImGui::Button("Create empty world")) {

						LOG(Info, "Not implemented yet");
					}

					if (ImGui::Button("Create empty world")) {

						LOG(Info, "Not implemented yet");
					}

					ImGui::EndPopup();
				}

				if (search_query.empty())
					show_current_folder_content(m_selected_directory);


				else {

					u32 index_buffer = 0;
					show_search_result_for_current_folder(m_selected_directory, index_buffer);

					if (index_buffer == 0) {
						UI::shift_cursor_pos(10, 10);
						ImGui::Text("Not item found containing [%s]", search_query.c_str());
					}

				}

				if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
					m_selected_items.reset();

		});
		ImGui::End();

		if (m_deletion_popup) {
			m_deletion_popup = false;
			m_deletion_consequenses.reset();
			m_block_mouse_input = false;

			if (!m_selected_items.empty()){
				
				get_deletion_consequenses(m_selected_items.main_item, m_deletion_consequenses);
				for (const auto x : m_selected_items.item_set)
					get_deletion_consequenses(x, m_deletion_consequenses);
					
			} else 
				get_deletion_consequenses(m_path_to_delete, m_deletion_consequenses);

			
			ImGui::OpenPopup("Deletion confirmation");
		}

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Always);
		if (ImGui::BeginPopupModal("Deletion confirmation", NULL, 0)) {

			if (m_deletion_consequenses.number_of_directoryies) {
				
				ImGui::TextWrapped("You are about to delete [%d] directory/s containing [%d] assets\nThis operation cannot be undone!", m_deletion_consequenses.number_of_directoryies, m_deletion_consequenses.number_of_files);
				if (m_deletion_consequenses.effected_assets.empty()) {

					ImGui::Image(m_warning_icon->get_descriptor_set(), ImVec2(15), ImVec2(0), ImVec2(1), ImVec4(.9f, .5f, 0.f, 1.f));
					ImGui::SameLine();
					ImGui::TextWrapped("Assets that are still in use but will be deleted:");
					ImGui::Indent();
					for (const auto name : m_deletion_consequenses.effected_assets)
						ImGui::TextWrapped("%s", name.c_str());
				}

			} else {

				ImGui::TextWrapped("You are about to delete [%d] assets\nThis operation cannot be undone!", m_deletion_consequenses.number_of_files);

				if (!m_deletion_consequenses.effected_assets.empty()) {
					
					ImGui::Image(m_warning_icon->get_descriptor_set(), ImVec2(15), ImVec2(0), ImVec2(1), ImVec4(.9f, .5f, 0.f, 1.f));
					ImGui::SameLine();
					ImGui::TextWrapped("The asset you are trying to delete is still in use\nThis operation cannot be undone!");
				} else
					ImGui::TextWrapped("Are you sure you want to delete this asset\nThis operation cannot be undone!");

			}

			UI::shift_cursor_pos(0, 10);
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0))) { 

				handel_deletion_action(m_path_to_delete);
				if (!m_selected_items.empty()){
					
					handel_deletion_action(m_selected_items.main_item);
					for (const auto x : m_selected_items.item_set)
						handel_deletion_action(x);
				}
				m_path_to_delete = std::filesystem::path();
				m_deletion_consequenses.reset();
				m_selected_items.reset();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { 
			
				m_path_to_delete = std::filesystem::path();
				m_deletion_consequenses.reset();
				ImGui::CloseCurrentPopup(); 
			}
			ImGui::EndPopup();
		}

	}

}
