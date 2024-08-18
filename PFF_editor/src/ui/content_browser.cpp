
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "mesh_import_window.h"
#include "PFF_editor.h"
#include "engine/resource_management/static_mesh_asset_manager.h"
#include "engine/resource_management/headers.h"
#include "engine/resource_management/mesh_headers.h"

#include "content_browser.h"

namespace PFF {

	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	static ImU32 background_color = IM_COL32(50, 50, 50, 255);

	// Function to display the search field and filtered results
	static std::string search_query;
	static void search_field_and_button(const std::filesystem::path& path) {

		ImVec2 input_text_size(180, 0); // Set width to 150 pixels, height to default
		ImGui::SetNextItemWidth(input_text_size.x);
		ImGui::InputText("##search_in_project_content_dir", search_query.data(), search_query.capacity(), ImGuiInputTextFlags_CharsNoBlank, NULL, NULL);
		ImGui::SameLine();
		if (ImGui::Button("search")) {

			CORE_LOG(Warn, "NOT INPLEMENTED YET");
		}
		ImGui::SameLine();

	}

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
		m_folder_image = create_ref<image>(PFF_editor::get().get_editor_executable_path() / "assets" / "icons" / "folder.png", image_format::RGBA);
		m_world_image = create_ref<image>(PFF_editor::get().get_editor_executable_path() / "assets" / "icons" / "world.png", image_format::RGBA);
		m_mesh_asset_image = create_ref<image>(PFF_editor::get().get_editor_executable_path() / "assets" / "icons" / "mesh_asset.png", image_format::RGBA);

		m_icon_size = { 60, 60 };
	}

	content_browser::~content_browser() {}


	void content_browser::select_new_directory(const std::filesystem::path& path) {

		m_selected_directory = path;
		m_partial_selected_directory = util::extract_path_from_project_folder(path);
	}


	void content_browser::show_directory_tree(const std::filesystem::path& path) {

		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (!entry.is_directory())
				continue;

			bool has_sub_folders = false;
			for (const auto& sub_entry : std::filesystem::directory_iterator(entry.path())) {		// <= HERE I GET AN ERROR
				if (!sub_entry.is_directory())
					continue;

				has_sub_folders = true;
				break;
			}


			if (has_sub_folders) {

				bool buffer = ImGui::TreeNodeEx(entry.path().filename().string().c_str(), base_flags);
				if (ImGui::IsItemClicked())
					select_new_directory(entry.path());
				
				drop_target_to_move_file(entry.path());

				if (buffer) {

					show_directory_tree(entry.path());
					ImGui::TreePop();
				}

			} else {

				ImGui::TreeNodeEx(entry.path().filename().string().c_str(), base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					select_new_directory(entry.path());

				drop_target_to_move_file(entry.path());
			}

		}

	}

	// Function to handle dropping files into the current directory
	void handle_drop(const std::filesystem::path& target_directory) {

		// Implement your file move logic here
		CORE_LOG(Info, "NOT IMPLEMENTED YET");
	}

	void content_browser::show_current_folder_content(const std::filesystem::path& path) {

		const ImGuiStyle& style = ImGui::GetStyle();
		const float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		const ImVec2 button_sz = m_icon_size; // Use the size of the icon for the button
		int count = 0;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (!entry.is_directory())
				continue;

			// Begin a new group for each item
			const std::string item_name = entry.path().filename().string();
			ImGui::BeginGroup();
			{
				ImGui::PushID(count++);

				ImGui::Image(m_folder_image->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::action_color_gray_active);

				ImVec2 text_size = ImGui::CalcTextSize(item_name.c_str());
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((m_icon_size.x - text_size.x) * 0.5f, 0));
				ImGui::TextWrapped(item_name.c_str());
				
				ImGui::PopID();
			}
			ImGui::EndGroup();
			switch (UI::get_mouse_interation_on_item()) {
			case UI::mouse_interation::double_click:
				select_new_directory(entry.path());
				return;

			case UI::mouse_interation::right_click:
				//ImGui::OpenPopup(popup_name.c_str());
				break;

			default:
				break;
			}


			// Handle drag source for files
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				ImGui::SetDragDropPayload("PROJECT_CONTENT_FOLDER", entry.path().string().c_str(), entry.path().string().length() + 1);

				ImGui::Image(m_folder_image->get_descriptor_set(), m_icon_size, ImVec2(0, 0), ImVec2(1, 1), UI::action_color_gray_active);
				ImGui::TextWrapped("%s", item_name.c_str());

				ImGui::EndDragDropSource();
			}


			// Handle dropping files into the current directory
			drop_target_to_move_file(entry.path());

			// handle item wrapping
			const float next_item_x2 = ImGui::GetItemRectMax().x + style.ItemSpacing.x + button_sz.x; // Expected position if next item was on the same line
			if (next_item_x2 < window_visible_x2)
				ImGui::SameLine();
		}





		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (entry.is_directory())
				continue;

			asset_file_header loc_asset_file_header;
			if (entry.path().extension() == ".pffasset") {

				serializer::binary(entry.path(), "PFF_asset_file", serializer::option::load_from_file)
					.entry(loc_asset_file_header);
			
			} else if (entry.path().extension() == ".pffworld") {

				serializer::yaml(entry.path(), "PFF_asset_file", serializer::option::load_from_file)
					.entry(KEY_VALUE(loc_asset_file_header.version))
					.entry(KEY_VALUE(loc_asset_file_header.type))
					.entry(KEY_VALUE(loc_asset_file_header.timestamp));
			}

			// Begin a new group for each item
			const std::string item_name = entry.path().filename().replace_extension("").string();
			ImGui::BeginGroup();
			{
				ImGui::PushID(count++);
				
				const ImVec2 item_pos = ImGui::GetCursorScreenPos();
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->AddRectFilled(item_pos, item_pos + m_icon_size, background_color, style.FrameRounding);

				switch (loc_asset_file_header.type) {

				case file_type::mesh:
					ImGui::Image(m_mesh_asset_image->get_descriptor_set(), m_icon_size);
					break;
					
				case file_type::world:
					ImGui::Image(m_world_image->get_descriptor_set(), m_icon_size);
					break;

				default:
					break;
				}


				std::string wrapped_text = UI::wrap_text_at_underscore(item_name, m_icon_size.x);
				ImGui::TextWrapped(wrapped_text.c_str());

				ImGui::PopID();
			}
			ImGui::EndGroup();
			const auto item_mouse_interation = UI::get_mouse_interation_on_item();

			const char* playload_name = "PROJECT_CONTENT_FILE";
			
			// TODO: make it possible to easyly tell the file type
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
				ImGui::SetDragDropPayload(playload_name, entry.path().string().c_str(), entry.path().string().length() + 1);

				ImGui::Image(m_mesh_asset_image->get_descriptor_set(), m_icon_size);
				ImGui::TextWrapped("%s", item_name.c_str());
								
				ImGui::EndDragDropSource();
			}
			
			// Context menu popup
			std::string popup_name = "item_context_menu_" + item_name;
			if (ImGui::BeginPopupContextItem(popup_name.c_str())) {

				if (ImGui::MenuItem("Rename")) {

					CORE_LOG(Info, "NOT IMPLEMENTED YET");
				}
				if (ImGui::MenuItem("Delete")) {
			
					std::error_code error_code{};
					CORE_VALIDATE(std::filesystem::remove(entry.path(), error_code), break, "deleting file from content folder: [" << entry.path() << "]", "FAILED to delete file from content folder: [" << entry.path() << "] error: " << error_code);
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

				default:
					break;
			}

			// handle item wrapping
			const float next_item_x2 = ImGui::GetItemRectMax().x + style.ItemSpacing.x + button_sz.x; // Expected position if next item was on the same line
			if (next_item_x2 < window_visible_x2)
				ImGui::SameLine();
		}


		// Handle dropping files into the current directory
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FOLDER")) {


			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FILE")) {
				std::string file_path(static_cast<const char*>(payload->Data));
				handle_drop(path / std::filesystem::path(file_path).filename());
			}
			ImGui::EndDragDropTarget();
		}
	}


	void content_browser::window() {

		//if (!show_window)
		//	return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::Begin("Content Browser", nullptr, window_flags);

		UI::custom_frame_NEW(350, true, UI::default_gray_1, [&]() {

			bool buffer = ImGui::TreeNodeEx((m_project_directory / CONTENT_DIR).filename().string().c_str(), base_flags | ImGuiTreeNodeFlags_DefaultOpen);
			if (ImGui::IsItemClicked())
				select_new_directory(m_project_directory / CONTENT_DIR);

			if (buffer) {

				show_directory_tree(m_project_directory / CONTENT_DIR);
				ImGui::TreePop();
			}

		}, [&]() {


			if (ImGui::Button(" import ##content_browser_import")) {

				std::filesystem::path source_path = util::file_dialog();

				if (source_path.extension() == ".gltf" || source_path.extension() == ".glb")
					PFF_editor::get().get_editor_layer()->add_window<mesh_import_window>(source_path, m_selected_directory);

				// TODO: add more import dialogs depending on extention

			}
			
			ImGui::SameLine();
			UI::shift_cursor_pos(30, 0);
			search_field_and_button(m_selected_directory);

			ImGui::SameLine();
			UI::shift_cursor_pos(10, 0);
			std::filesystem::path current_path;
			for (const auto& part : m_partial_selected_directory) {

				current_path /= part;
				if (UI::gray_button(part.string().c_str())) {

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


			show_current_folder_content(m_selected_directory);

		});


		ImGui::End();
	}

}
