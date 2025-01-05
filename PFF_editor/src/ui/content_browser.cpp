
#include "util/pch_editor.h"

#include <imgui_internal.h>
#include <util/ui/pannel_collection.h>
#include <engine/resource_management/headers.h>
#include <engine/resource_management/mesh_headers.h>
#include <engine/resource_management/static_mesh_asset_manager.h>

#include "mesh_import_window.h"
#include "PFF_editor.h"
#include "editor_layer.h"

#include "content_browser.h"

namespace PFF {

	static ImGuiTreeNodeFlags	base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	static ImU32				background_color = IM_COL32(50, 50, 50, 255);
	static std::string			search_query;
	const ImVec2				folder_open_icon_size(18, 14);																		// Calculate the icon size for the tree (make it smaller than regular icons)
	const ImVec2				folder_closed_icon_size(18, 17);																	// Calculate the icon size for the tree (make it smaller than regular icons)
	ImGuiWindow*				folder_display_window{};

	const std::vector<std::pair<std::string, std::string>> import_files = {
		{"glTF 2.0 file", "*.gltf;*.glb"},
	};

	int hash_path(const std::filesystem::path& path) { return static_cast<int>(std::hash<std::string>()(path.string())); }


	static void drop_target_to_move_file(const std::filesystem::path folder_path) {

		if (ImGui::BeginDragDropTarget()) {

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FILE")) {

				const std::filesystem::path file_path = (const char*)payload->Data;
				try {

					std::filesystem::path destination = folder_path / file_path.filename();
					std::filesystem::rename(file_path, destination);
					CORE_LOG(Info, "File moved successfully!");

				} catch (const std::filesystem::filesystem_error& e)
					CORE_LOG(Error, "Error: " << e.what());

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

		// Implement your file move logic here
		CORE_LOG(Info, "NOT IMPLEMENTED YET");
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

		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (!entry.is_directory())
				continue;

			// Begin a new group for each item
			const int current_ID = hash_path(entry.path());
			const std::string item_name = entry.path().filename().string();
			const ImVec4& color = (m_selected_items.find(entry.path()) != m_selected_items.end()) ? UI::get_action_color_00_active_ref() : UI::get_action_color_gray_active_ref();
			ImGui::BeginGroup();
			{
				ImGui::PushID(current_ID);
				ImGui::Image(m_folder_big_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color);
				ImVec2 text_size = ImGui::CalcTextSize(item_name.c_str());
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((m_icon_size.x - text_size.x) * 0.5f, 0));
				ImGui::TextWrapped(item_name.c_str());
				ImGui::PopID();
			}
			ImGui::EndGroup();

			switch (UI::get_mouse_interation_on_item()) {
				case UI::mouse_interation::double_click:
					CORE_LOG(Trace, "Set TreeNode to open ID[" << current_ID << "]");
					ImGui::TreeNodeSetOpen(folder_display_window->GetID(current_ID), true);
					select_new_directory(entry.path());
					return;

				case UI::mouse_interation::right_click:
					// TODO: open popup					ImGui::OpenPopup(popup_name.c_str());
					break;

				case UI::mouse_interation::single_click:
					if (ImGui::GetIO().KeyShift) {

						if (m_selected_items.find(entry.path()) == m_selected_items.end())				// If Shift is held, select the item
							m_selected_items.insert(entry.path());
						else
							m_selected_items.erase(entry.path());

					} else {
						// Clear selection and select the clicked item
						m_selected_items.clear();
						m_selected_items.insert(entry.path());
					}
					break;

				case UI::mouse_interation::hovered:
					// TODO: draw rectangle behind item
					break;

				default: break;
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {

				const std::string path_string = entry.path().string();
				ImGui::SetDragDropPayload("PROJECT_CONTENT_FOLDER", path_string.c_str(), path_string.length() + 1);
				CORE_LOG(Trace, "DRAG-DROP  FOLDER");
				ImGui::Image(m_folder_big_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::get_action_color_gray_active_ref());
				ImGui::TextWrapped("%s", item_name.c_str());
				ImGui::EndDragDropSource();
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

			const int current_ID = hash_path(entry.path());
			display_file(entry.path(), current_ID);
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
				CORE_LOG(Error, "file_path: " << file_path);
				handle_drop(path / std::filesystem::path(file_path).filename());
			}
			ImGui::EndDragDropTarget();
		}
	}

	void content_browser::display_file(const std::filesystem::path file_path, int ID) {

		asset_file_header loc_asset_file_header;
		if (file_path.extension() == ".pffasset") {

			serializer::binary(file_path, "PFF_asset_file", serializer::option::load_from_file)
				.entry(loc_asset_file_header);

		} else if (file_path.extension() == ".pffworld") {

			serializer::yaml(file_path, "PFF_asset_file", serializer::option::load_from_file)
				.entry(KEY_VALUE(loc_asset_file_header.file_version))
				.entry(KEY_VALUE(loc_asset_file_header.type))
				.entry(KEY_VALUE(loc_asset_file_header.timestamp));
		}

		// Begin a new group for each item
		const ImVec4& color = (file_path == m_main_selected_item) ? UI::get_action_color_00_active_ref() : (m_selected_items.find(file_path) != m_selected_items.end()) ? UI::get_action_color_00_faded_ref() : ImVec4(1);
		const std::string item_name = file_path.filename().replace_extension("").string();
		ImGui::BeginGroup();
		{
			ImGui::PushID(ID);

			const ImVec2 item_pos = ImGui::GetCursorScreenPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(item_pos, item_pos + m_icon_size, background_color, ImGui::GetStyle().FrameRounding);

			switch (loc_asset_file_header.type) {
				case file_type::mesh:
					ImGui::Image(m_mesh_asset_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color);
					break;

				case file_type::world:
					ImGui::Image(m_world_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), color);
					break;

				default: break;
			}

			std::string wrapped_text = UI::wrap_text_at_underscore(item_name, m_icon_size.x);
			ImGui::TextWrapped(wrapped_text.c_str());

			ImGui::PopID();
		}
		ImGui::EndGroup();
		const auto item_mouse_interation = UI::get_mouse_interation_on_item();

		// TODO: make it possible to easyly tell the file type
		const char* playload_name = "PROJECT_CONTENT_FILE";
		//switch (loc_asset_file_header.type) {
		//case file_type::mesh:
		//	playload_name = "PROJECT_CONTENT_FILE_MESH";
		//	break;
		//case file_type::world:
		//	playload_name = "PROJECT_CONTENT_FILE_WORLD";
		//	break;
		//default:
		//	break;
		//}

		// Handle drag source for files
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {

			if (m_selected_items.empty()) {

				const std::string path_string = file_path.string();
				ImGui::SetDragDropPayload(playload_name, path_string.c_str(), path_string.length() + 1);
				ImGui::Image(m_mesh_asset_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::get_action_color_gray_active_ref());
				ImGui::TextWrapped("%s", item_name.c_str());
				ImGui::EndDragDropSource();

			} else {

				const std::string path_string = file_path.string();
				ImGui::SetDragDropPayload("PROJECT_CONTENT_FILE_MULTI", &m_selected_items, sizeof(&m_selected_items));
				ImGui::Image(m_mesh_asset_icon->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::get_action_color_gray_active_ref());
				ImGui::TextWrapped("[%ld] files", m_selected_items.size());
				ImGui::EndDragDropSource();

			}

		}

		// Context menu popup
		std::string popup_name = "item_context_menu_" + item_name;
		if (ImGui::BeginPopupContextItem(popup_name.c_str())) {

			if (ImGui::MenuItem("Rename"))
				CORE_LOG(Info, "NOT IMPLEMENTED YET");

			if (ImGui::MenuItem("Delete")) {

				std::error_code error_code{};
				CORE_VALIDATE(std::filesystem::remove(file_path, error_code), return, "deleting file from content folder: [" << file_path << "]", "FAILED to delete file from content folder: [" << file_path << "] error: " << error_code);
			}


			if (item_mouse_interation == UI::mouse_interation::none && !UI::is_holvering_window())
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		switch (item_mouse_interation) {
			case UI::mouse_interation::double_click:
				CORE_LOG(Info, "NOT IMPLEMENTED YET => should opening coresponding editor window");
				break;

			case UI::mouse_interation::right_click:
				ImGui::OpenPopup(popup_name.c_str());
				break;

			case UI::mouse_interation::single_click:

				if (ImGui::GetIO().KeyShift) {

					if (!m_main_selected_item.empty())
						m_selected_items.insert(m_main_selected_item);
					m_main_selected_item = file_path;
					m_selected_items.insert(file_path);

				} else if (ImGui::GetIO().KeyCtrl) {

					if (m_selected_items.find(file_path) == m_selected_items.end())				// If Shift is held, select the item
						m_selected_items.insert(file_path);
					else
						m_selected_items.erase(file_path);

				} else {

					m_selected_items.clear();
					m_main_selected_item = file_path;
				}
				break;

			default: break;
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


			if (ImGui::Button(" import ##content_browser_import")) {

				std::filesystem::path source_path = util::file_dialog("Import asset", import_files);

				if (source_path.extension() == ".gltf" || source_path.extension() == ".glb")
					PFF_editor::get().get_editor_layer()->add_window<mesh_import_window>(source_path, m_selected_directory);

				// TODO: add more import dialogs depending on extention

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

			//ImGui::ShowDemoWindow();

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


		});


		ImGui::End();
	}

}
