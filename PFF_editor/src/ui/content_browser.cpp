
#include "util/pch_editor.h"

#include <imgui_internal.h>
#include <util/ui/pannel_collection.h>
#include <engine/resource_management/general_resource_manager.h>
#include <engine/resource_management/headers.h>
#include <engine/resource_management/mesh_headers.h>
#include <engine/resource_management/static_mesh_asset_manager.h>

#include "ui/import/mesh_import_window.h"
#include "ui/import/texture_import_window.h"
#include "ui/creation/material_inst_create_window.h"
#include "util/import_dialog.h"
#include "PFF_editor.h"
#include "editor_layer.h"

#include "content_browser.h"





#include "engine/resource_management/texture_serializer.h"

#define ITEM_DRAD_DRAP_SOURCE_NAME_LENGTH			9



namespace PFF {

	static ImGuiTreeNodeFlags	base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	static ImU32				background_color = IM_COL32(50, 50, 50, 255);
	static std::string			search_query;
	const ImVec2				folder_open_icon_size(18, 14);																		// Calculate the icon size for the tree (make it smaller than regular icons)
	const ImVec2				folder_closed_icon_size(18, 17);																	// Calculate the icon size for the tree (make it smaller than regular icons)
	ImGuiWindow*				folder_display_window{};

	// enum file_curruption_source {
	// 	unknown = 0,
	// 	empty_file,
	// 	header_incorrect,
	// };

	const std::vector<std::pair<std::string, std::string>> posible_import_tile_types = {
		{"All supported file types",    "*.gltf;*.glb;*.png;*.jpg;*.jpeg;*.jpe;*.tga;*.bmp;*.psd;*.gif;*.hdr;*.pic;*.ppm;*.pgm"},
		{"glTF 2.0 files",              "*.gltf;*.glb"},
		{"Images",                 		"*.png;*.jpg;*.jpeg;*.jpe;*.tga;*.bmp;*.psd;*.gif;*.hdr;*.pic;*.ppm;*.pgm"},					// all images
		{"JPEG images",                 "*.jpg;*.jpeg;*.jpe"},
		{"PNG images",                  "*.png"},
		{"TGA images",                  "*.tga"},
		{"BMP images",                  "*.bmp"},
		{"PSD images",                  "*.psd"},
		{"GIF images",                  "*.gif"},
		{"HDR (Radiance .hdr)",         "*.hdr"},
		{"Softimage PIC",               "*.pic"},
		{"PNM (PPM/PGM)",               "*.ppm;*.pgm"},
	};

	// ================================================ util ================================================

	FORCEINLINE int hash_path(const std::filesystem::path& path) { return (int)std::filesystem::hash_value(path); }				// hash_value() return a site_t (can be cast to int)


	FORCEINLINE bool is_file_used(const std::filesystem::path& file_path) {

		resource_manager::asset_curruption_source loc_file_curruption_source = resource_manager::asset_curruption_source::unknown;
		asset_file_header loc_asset_file_header;
		if (!resource_manager::try_to_deserialize_file_header(file_path, false, loc_file_curruption_source, loc_asset_file_header))
			return false;							// assuming currupted files are nover used

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


	FORCEINLINE void get_deletion_consequenses(const std::filesystem::path& file_path, deletion_consequenses& deletion_consequenses) {
		
		if (std::filesystem::is_directory(file_path)) {

			deletion_consequenses.number_of_directoryies++;
			ref_check_files_and_sub_directory(file_path, deletion_consequenses);
		
		} else {

			deletion_consequenses.number_of_files++;
			if (is_file_used(file_path))
				deletion_consequenses.effected_assets.insert(file_path.filename().replace_extension("").string());
		}
	}
	
	
	// Function to handle dropping files into the current directory
	FORCEINLINE void handle_drop(const std::filesystem::path& target_directory, const std::filesystem::path& file) {

		// Implement file move logic here
		LOG(Debug, "dropping file: " << file.generic_string() << " into target dir: " << target_directory.generic_string());
					
		const std::filesystem::path absolute_path = application::get().get_project_path() / CONTENT_DIR / file;
		std::filesystem::path destination = target_directory / file.filename();
		try {
			std::filesystem::rename(absolute_path, destination);										// move the file/directory in one atomic operation
		
		} catch (const std::filesystem::filesystem_error& e) {

			if (e.code() == std::errc::cross_device_link) {							// On some platforms, rename() fails if crossing devices → fallback to copy+remove
				if (std::filesystem::is_directory(absolute_path)) {									// If it's a directory, use recursive copy

					std::filesystem::copy(absolute_path, destination, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
					std::filesystem::remove_all(absolute_path);

				} else {
					std::filesystem::copy_file(absolute_path, destination, std::filesystem::copy_options::overwrite_existing);
					std::filesystem::remove(absolute_path);
				}
			}
			else {
				// Propagate other errors
				throw;
			}
		}
	}


	FORCEINLINE static void drop_target_to_move_file(const std::filesystem::path& folder_path) {
		
		if (ImGui::BeginDragDropTarget()) {

#define DROP_FILE(type)		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type)) {							\
								const std::filesystem::path file_path = (const char*)payload->Data;							\
								handle_drop(folder_path, file_path); }
			DROP_FILE(DRAG_DROP_CONTENT_BROWSER_MESH)
			DROP_FILE(DRAG_DROP_CONTENT_BROWSER_TEXTURE)
			DROP_FILE(DRAG_DROP_CONTENT_BROWSER_MATERIAL)
			DROP_FILE(DRAG_DROP_CONTENT_BROWSER_MATERIAL_INST)
			DROP_FILE(DRAG_DROP_CONTENT_BROWSER_WORLD)
#undef DROP_FILE
			
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DRAG_DROP_CONTENT_BROWSER_MULTI)) {

				util::selected_items<std::filesystem::path>** received_ptr = static_cast<util::selected_items<std::filesystem::path>**>(payload->Data);
				util::selected_items<std::filesystem::path>* file_paths = *received_ptr;
				for (const std::filesystem::path path : (*file_paths).item_set)
					handle_drop(folder_path, path);
			}
			ImGui::EndDragDropTarget();
		}
	}
	
	// ================================================ util ================================================


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
		m_texture_icon = 		editor_layer->get_texture_icon();
		m_material_icon = 		editor_layer->get_material_icon();
		m_material_inst_icon = 	editor_layer->get_material_inst_icon();

		m_icon_size = { 60, 60 };
		m_icon_padding = { 4, 4 };
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
	

	FORCEINLINE void content_browser::wrapp_displayed_items(f32& max_text_height, const ImVec2 text_size, const ImVec2 item_padding, const f32 window_area_x) {
		
		const ImGuiStyle& style = ImGui::GetStyle();
		if (text_size.y > max_text_height)
			max_text_height = text_size.y;

		const float next_item_x2 = ImGui::GetItemRectMax().x + style.ItemSpacing.x + m_icon_size.x + (2*item_padding.x);		// Expected position if next item was on the same line
		if (next_item_x2 < window_area_x) {														// handle item wrapping
			ImGui::SameLine();
			UI::shift_cursor_pos(item_padding.x *2, 0);
		} else {
			UI::shift_cursor_pos(0, item_padding.y *3); //  + max_text_height);
			// LOG(Trace, "max_text_height: " << max_text_height << "  text_size.y: " << text_size.y << "    Y: " << item_padding.y + max_text_height - text_size.y);
			max_text_height = 0;																								// reset when entering new line
		}
	}

	//static std::string search_query;
	void content_browser::show_search_result_for_current_folder(const std::filesystem::path& path, u32& item_index) {

		f32 max_text_height{};
		ImVec2 text_height{};
		const float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (entry.is_directory())
				show_search_result_for_current_folder(entry.path(), item_index);			// look into any subfolder
			else {

				if (entry.path().filename().string().find(search_query) == std::string::npos)
					continue;

				display_file(entry.path(), item_index++, text_height);

				wrapp_displayed_items(max_text_height, text_height, m_icon_padding, window_visible_x2);
				// // handle item wrapping
				// const float next_item_x2 = ImGui::GetItemRectMax().x + ImGui::GetStyle().ItemSpacing.x + m_icon_size.x; // Expected position if next item was on the same line
				// if (next_item_x2 < window_visible_x2)
				// 	ImGui::SameLine();
			}
		}
	}


	void content_browser::show_current_folder_content(const std::filesystem::path& path) {

		const ImGuiStyle& style = ImGui::GetStyle();
		const float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		u32 count = 0;

		f32 max_text_height = 0;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {											// display directorys first

			if (!entry.is_directory())
				continue;

			// Begin a new group for each item
			const int current_ID = hash_path(entry.path());
			std::string item_name = entry.path().filename().string();
			UI::wrap_text(item_name, m_icon_size.x, m_max_number_of_lines_in_displayed_title);
			// const std::string wrapped_text = 
			ImVec2 text_size{};
			const std::string popup_name = "dir_context_menu_" + item_name;
			const ImVec4& color = (entry.path() == m_selected_directories.main_item) ? UI::get_action_color_00_active_ref() : (m_selected_directories.item_set.find(entry.path()) != m_selected_directories.item_set.end()) ? UI::get_action_color_00_faded_ref() : UI::get_action_color_gray_hover_ref(); // ImGui::GetStyle().Colors[ImGuiCol_ChildBg];
			ImGui::BeginGroup();
			{
				ImGui::PushID(current_ID);
				ImGui::Image(m_folder_big_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color);
				text_size = ImGui::CalcTextSize(item_name.c_str());
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((m_icon_size.x - text_size.x) * 0.5f, 0));
				ImGui::TextWrapped("%s", item_name.c_str());
				ImGui::PopID();
			}
			ImGui::EndGroup();
			drop_target_to_move_file(entry.path());


			const auto dir_mouse_interation = UI::get_mouse_interation_on_item(m_block_mouse_input);
			util::process_selection_input<std::filesystem::path>(m_selected_directories, dir_mouse_interation, entry.path(), popup_name.c_str(), [&]() {

				std::vector<std::filesystem::path> files_in_dir;
				for (const auto& entry : std::filesystem::directory_iterator(m_selected_directory))
					files_in_dir.push_back(entry.path());

				auto it_main = std::find(files_in_dir.begin(), files_in_dir.end(), m_selected_directories.main_item);
				auto it_clicked = std::find(files_in_dir.begin(), files_in_dir.end(), entry.path());
				if (it_main != files_in_dir.end() && it_clicked != files_in_dir.end()) {

					auto start = std::min(it_main, it_clicked);
					auto end = std::max(it_main, it_clicked);
					m_selected_directories.item_set.clear();
					for (auto it = start; it != std::next(end); ++it)
						m_selected_directories.item_set.insert(*it);
				}
			});


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

			wrapp_displayed_items(max_text_height, text_size, ImVec2(0), window_visible_x2);
			
			// const float next_item_x2 = ImGui::GetItemRectMax().x + style.ItemSpacing.x;		// Expected position if next item was on the same line
			// if (next_item_x2 < window_visible_x2)															// handle item wrapping
			// 	ImGui::SameLine();
		}
		UI::shift_cursor_pos(m_icon_padding.x, 0);

		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (entry.is_directory())
				continue;

			const std::filesystem::path current_path = entry.path();
			const int current_ID = hash_path(entry.path());
			ImVec2 text_size{};
			display_file(current_path, current_ID, text_size);

			wrapp_displayed_items(max_text_height, text_size, m_icon_padding, window_visible_x2);
		}

		// Handle dropping files into the current directory
		// if (ImGui::BeginDragDropTarget()) {
		// 	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FOLDER")) {

		// 		// Handle folder drop
		// 	}
		// 	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FILE")) {
		// 		std::string file_path(static_cast<const char*>(payload->Data));
		// 		LOG(Error, "file_path: " << file_path);
		// 		handle_drop(path / std::filesystem::path(file_path).filename());
		// 	}
		// 	ImGui::EndDragDropTarget();
		// }

		logged_warning_for_current_folder = true;
	}


	void content_browser::display_file(const std::filesystem::path& file_path, int ID, ImVec2& text_size) {

		const char* drag_drop_source = "PROJECT_CONTENT_FILE";
		const size_t hash_value = std::filesystem::hash_value(file_path);
		resource_manager::asset_curruption_source loc_file_curruption_source = resource_manager::asset_curruption_source::unknown;
		asset_file_header loc_asset_file_header;
		bool file_deserialized = resource_manager::try_to_deserialize_file_header(file_path, !logged_warning_for_current_folder, loc_file_curruption_source, loc_asset_file_header);

		if (file_deserialized) {

			if (m_asset_timestamp.contains(hash_value)) {

				const auto asset_timestamp = m_asset_timestamp[hash_value];
				if (asset_timestamp < loc_asset_file_header.timestamp) {

					LOG(Info, "Found reimported asset")
					PFF_editor::get().get_icon_manager_ref().request_icon_refresh(file_path);
					m_asset_timestamp[hash_value] = loc_asset_file_header.timestamp;						// update time
				}

			} else {
				m_asset_timestamp[hash_value] = loc_asset_file_header.timestamp;
			}
		}


		// Begin a new group for each item
		const ImVec4& color = (file_path == m_selected_items.main_item) ? UI::get_action_color_00_active_ref() : (m_selected_items.item_set.find(file_path) != m_selected_items.item_set.end()) ? UI::get_action_color_00_faded_ref() : UI::get_action_color_gray_hover_ref(); // ImGui::GetStyle().Colors[ImGuiCol_ChildBg];
		std::string item_name = file_path.filename().replace_extension("").string();
		// const std::string wrapped_text = 
		UI::wrap_text(item_name, m_icon_size.x, m_max_number_of_lines_in_displayed_title);
		ImGui::PushID(ID);
		ImGui::BeginGroup();
		{

			const ImVec2 item_pos = ImGui::GetCursorScreenPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			text_size = ImGui::CalcTextSize(item_name.c_str());
			draw_list->AddRectFilled(item_pos - m_icon_padding, item_pos + m_icon_size + m_icon_padding + ImVec2(0, text_size.y + ImGui::GetStyle().ItemSpacing.y*2), ImGui::GetColorU32(color), ImGui::GetStyle().FrameRounding);

			if (!file_deserialized) {

				ImGui::Image(m_warning_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), (file_path == m_selected_items.main_item) ? ImVec4(.9f, .5f, 0.f, 1.f) : (m_selected_items.item_set.find(file_path) != m_selected_items.item_set.end()) ? ImVec4(.8f, .4f, 0.f, 1.f) : ImVec4(1.f, .6f, 0.f, 1.f));

			} else {

				switch (loc_asset_file_header.type) {
					case file_type::mesh:
						ImGui::Image(m_mesh_asset_icon->get_descriptor_set(), m_icon_size);
						drag_drop_source = DRAG_DROP_CONTENT_BROWSER_MESH;
						break;

					case file_type::world:
						ImGui::Image(m_world_icon->get_descriptor_set(), m_icon_size);
						drag_drop_source = DRAG_DROP_CONTENT_BROWSER_WORLD;
						break;

					case file_type::material:
						ImGui::Image(m_material_icon->get_descriptor_set(), m_icon_size);
						drag_drop_source = DRAG_DROP_CONTENT_BROWSER_MATERIAL;
						break;

					case file_type::material_instance:
						ImGui::Image(m_material_inst_icon->get_descriptor_set(), m_icon_size);
						drag_drop_source = DRAG_DROP_CONTENT_BROWSER_MATERIAL_INST;
						break;

					case file_type::texture: {

						drag_drop_source = DRAG_DROP_CONTENT_BROWSER_TEXTURE;
						auto icon = PFF_editor::get().get_icon_manager_ref().request_icon(file_path);
						if (icon) {

							ImGui::Image(icon->get_descriptor_set(), m_icon_size);
							break;
						}
							
						LOG(Trace, "request an icon for [" << item_name << "]")
						ImGui::Image(m_texture_icon->get_descriptor_set(), m_icon_size);
						break;

					} break;

					default:

						// ============================== DEV-ONLY (using yaml for serialization as test in material instance) ==============================
						drag_drop_source = DRAG_DROP_CONTENT_BROWSER_MATERIAL_INST;
						// ============================== DEV-ONLY (using yaml for serialization as test in material instance) ==============================


						file_deserialized = false;																		// mark as currupted
						loc_file_curruption_source = resource_manager::asset_curruption_source::header_incorrect;		// maks as currupted
						std::string faulty_file_name = file_path.filename().generic_string();
						if (!logged_warning_for_current_folder)
							LOG(Warn, "file [" << faulty_file_name << "] cound not be identified, detected asset header type [" << asset_header_file_type_to_str(loc_asset_file_header.type) << "]");
						ImGui::Image(m_warning_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), (file_path == m_selected_items.main_item) ? ImVec4(.9f, .5f, 0.f, 1.f) : (m_selected_items.item_set.find(file_path) != m_selected_items.item_set.end()) ? ImVec4(.8f, .4f, 0.f, 1.f) : ImVec4(1.f, .6f, 0.f, 1.f));
						break;
				}
			}

			ImGui::Text("%s", item_name.c_str());

		}
		ImGui::EndGroup();
		ImGui::PopID();
		auto item_mouse_interation = UI::get_mouse_interation_on_item(m_block_mouse_input);

		// Handle drag source for files
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {

			// const char* drag_drop_source = "PROJECT_CONTENT_FILE";
			// LOG(Trace, "m_selected_items count: " << m_selected_items.size());
			if (m_selected_items.item_set.empty()) {

				const std::string path_string = util::extract_path_from_project_content_folder(file_path).string();
				ImGui::SetDragDropPayload(drag_drop_source, path_string.c_str(), path_string.length() + 1);

				switch (loc_asset_file_header.type) {
					case file_type::mesh:				ImGui::Image(m_mesh_asset_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color); break;
					case file_type::world:				ImGui::Image(m_world_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color); break;
					case file_type::material:			ImGui::Image(m_material_icon->get_descriptor_set(), m_icon_size); break;
					case file_type::material_instance:	ImGui::Image(m_material_inst_icon->get_descriptor_set(), m_icon_size); break;
					case file_type::texture: {
						auto icon = PFF_editor::get().get_icon_manager_ref().request_icon(file_path);
						if (icon) {

							ImGui::Image(icon->get_descriptor_set(), m_icon_size);
							break;
						}
							
						LOG(Warn, "request an icon")
						ImGui::Image(m_texture_icon->get_descriptor_set(), m_icon_size);
						break;
					}
					[[fallthrough]];			// if image cant be found just show the warning image
					default:					ImGui::Image(m_warning_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), (file_path == m_selected_items.main_item) ? ImVec4(.9f, .5f, 0.f, 1.f) : (m_selected_items.item_set.find(file_path) != m_selected_items.item_set.end()) ? ImVec4(.8f, .4f, 0.f, 1.f) : ImVec4(1.f, .6f, 0.f, 1.f)); break;
				}

				ImGui::Text("%s", item_name.c_str());
				ImGui::EndDragDropSource();

			} else {

				util::selected_items<std::filesystem::path>* payload_data = &m_selected_items;
				ImGui::SetDragDropPayload(DRAG_DROP_CONTENT_BROWSER_MULTI, &payload_data, sizeof(payload_data));
				ImGui::Image(m_mesh_asset_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::get_action_color_gray_active_ref());
				ImGui::Text("[%ld] files", m_selected_items.item_set.size() + (m_selected_items.main_item.empty() ? 0 : 1));
				ImGui::EndDragDropSource();
			}
		}

		ImVec2 expected_size = ImVec2(20, 50);						// Adjust based on content
		if (!file_deserialized)
			switch (loc_file_curruption_source) {					// aprocimate size by corruption type
				default:
				case resource_manager::asset_curruption_source::unknown:				expected_size = ImVec2(280, 250); break;	// should display everything to help user
				case resource_manager::asset_curruption_source::header_incorrect:		expected_size = ImVec2(280, 250); break;	// should display header
				case resource_manager::asset_curruption_source::empty_file:				expected_size = ImVec2(180, 150); break;	// dosnt need to display anything other than size
			}
		UI::adjust_popup_to_window_bounds(expected_size);
		std::string popup_name = "item_context_menu_" + item_name;
		if (ImGui::BeginPopupContextItem(popup_name.c_str())) {

			if (ImGui::MenuItem("Rename"))
				LOG(Info, "NOT IMPLEMENTED YET");

			if (ImGui::MenuItem("Delete")) {								// open popup to display consequences of deleting file and ask again
	
				m_deletion_popup = true;
				m_block_mouse_input = true;
				if (m_selected_items.main_item.empty() && m_selected_items.item_set.empty())
					m_path_to_delete = file_path;
			}

			if (!file_deserialized) {											// display everything we know about the file to help user find error

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
				ImGui::Image(m_warning_icon->get_descriptor_set(), ImVec2(15), ImVec2(0, 0), ImVec2(1, 1), ImVec4(.9f, .5f, 0.f, 1.f));
				ImGui::SameLine();
				ImGui::Text("file is corrupted:");

				switch (loc_file_curruption_source) {
					default:
					case resource_manager::asset_curruption_source::unknown:				ImGui::Text("Unknown reason for detecting corrupted file"); break;
					case resource_manager::asset_curruption_source::header_incorrect:		ImGui::Text("File corrupted because deserialized header type is incorrect"); break;
					case resource_manager::asset_curruption_source::empty_file:			ImGui::Text("File is empty and can't be used by game engine"); break;
				}

				ImGui::Text("general data");

				UI::begin_table("currupted_file_data_table", false);
				std::error_code error_code;
				const std::string_view file_size_in_MB = util::to_string((f32)std::filesystem::file_size(file_path, error_code) / 1024) + " MB";
				UI::table_row("file size", file_size_in_MB);
				ImGui::EndTable();

				if (loc_file_curruption_source != resource_manager::asset_curruption_source::empty_file) {

					ImGui::Text("header data");

					UI::begin_table("currupted_file_data_table", false);
					UI::table_row("version", loc_asset_file_header.file_version.to_str() );
					UI::table_row("file type", asset_header_file_type_to_str(loc_asset_file_header.type) );
					UI::table_row("timestamp", loc_asset_file_header.timestamp.to_str() );
					ImGui::EndTable();
				}

			}

			m_block_mouse_input = UI::is_holvering_window();
			if (!m_block_mouse_input)
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		util::process_selection_input<std::filesystem::path>(m_selected_items, item_mouse_interation, file_path, popup_name.c_str(), [&]() {

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
		});
	}


	void content_browser::handel_deletion_action(const std::filesystem::path& file_path) {

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

		// if (!show_window)
		// 	return;

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

					std::vector<std::filesystem::path> source_paths = util::file_dialog_multi("Import asset", posible_import_tile_types);

					std::vector<std::filesystem::path> image_source_paths{};
					std::vector<std::filesystem::path> mesh_source_paths{};
					for (const auto path : source_paths) {

						if (path.extension() == ".png" 	||
							path.extension() == ".jpg" 	||
							path.extension() == ".jpeg" ||
							path.extension() == ".jpe" 	||
							path.extension() == ".tga" 	||
							path.extension() == ".bmp" 	||
							path.extension() == ".psd" 	||
							path.extension() == ".gif" 	||
							path.extension() == ".hdr" 	||
							path.extension() == ".pic" 	||
							path.extension() == ".ppm" 	||
							path.extension() == ".pgm")
								image_source_paths.push_back(path);

						else if (path.extension() == ".gltf" || path.extension() == ".glb")
							mesh_source_paths.push_back(path);
					}

					PFF_editor::get().get_editor_layer()->add_window<texture_import_window>(std::move(image_source_paths), m_selected_directory);

					for (const auto path : source_paths) {

						if (path.extension() == ".gltf" || path.extension() == ".glb") {
							
							LOG(Debug, "TODO: modefy mesh import window to accept multiple paths");
							PFF_editor::get().get_editor_layer()->add_window<mesh_import_window>(path, m_selected_directory);
						}
										
						else
							LOG(Warn, "Tryed to import unsupported file type")										// TODO: add a notification system to main window
					}
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

				if (UI::mouse_interation::right_clicked == UI::get_mouse_interation_on_window())
					ImGui::OpenPopup("##content_browser_current_dir_popup");

				UI::adjust_popup_to_window_bounds(ImVec2(200, 200));
				if (ImGui::BeginPopup("##content_browser_current_dir_popup")) {


					if (UI::gray_button("create directory")) {

						if (std::filesystem::exists(m_selected_directory / "new_dir_")) {
							u32 counter = 0;
							std::stringstream dir_name{};
							dir_name << "new_dir_" << util::to_string(counter);
							while (std::filesystem::exists(m_selected_directory / dir_name.str()))  {
								counter++;
								dir_name.str("");
								dir_name.flush();
								dir_name << "new_dir_" << util::to_string(counter);
							}
							std::filesystem::create_directory(m_selected_directory / dir_name.str());
						} else 
							std::filesystem::create_directory(m_selected_directory / "new_dir_");

						ImGui::CloseCurrentPopup();
					}

					ImGui::Separator();
					ImGui::Text("Create New asset");

					if (ImGui::BeginMenu("world")) {

						if (UI::gray_button("Create empty world")) {

							LOG(Info, "Not implemented yet");
						}

						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("material")) {

						if (UI::gray_button("Create Material")) {

							LOG(Info, "Not implemented yet");
						}
	
						if (UI::gray_button("Create Material Instance")) {
								
							PFF_editor::get().get_editor_layer()->add_window<material_inst_create_window>(m_selected_directory);
						}
						ImGui::EndMenu();
					}

					ImGui::EndPopup();
				}

				if (search_query.empty()) {
					UI::shift_cursor_pos(0, m_icon_padding.y);
					show_current_folder_content(m_selected_directory);
				}


				else {

					u32 index_buffer = 0;
					show_search_result_for_current_folder(m_selected_directory, index_buffer);

					if (index_buffer == 0) {
						UI::shift_cursor_pos(10, 10);
						ImGui::Text("Not item found containing [%s]", search_query.c_str());
					}

				}

				if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {

					m_selected_items.reset();
					m_selected_directories.reset();
				}

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

		
		ImGuiWindowFlags popup_window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Appearing);
		if (ImGui::BeginPopupModal("Deletion confirmation", NULL, popup_window_flags)) {

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
				m_selected_directories.reset();
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
