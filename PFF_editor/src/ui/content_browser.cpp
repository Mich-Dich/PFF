
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "mesh_import_window.h"
#include "PFF_editor.h"
#include "engine/resource_management/static_mesh_asset_manager.h"

#include "content_browser.h"

namespace PFF {

	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	static ImU32 background_color = IM_COL32(70, 70, 70, 155);

	static std::filesystem::path extract_from_folder(const std::filesystem::path& full_path) {

		std::string full_path_str = full_path.string();
		std::string folder_marker = std::string(CONTENT_DIR);
		CORE_LOG(Trace, "folder_marker: " << folder_marker << " full_path_str: " << full_path_str);

		size_t pos = full_path_str.find(folder_marker);

		if (pos != std::string::npos) {
			std::string result_str = full_path_str.substr(pos);
			return std::filesystem::path(result_str);
		} else {

			CORE_LOG(Trace, "NOT FOUND");
			return {};
		}
	}

	// Function to display the search field and filtered results
	static std::string search_query;
	static void DisplaySearchFieldAndButtons(const std::filesystem::path& path) {

		ImVec2 input_text_size(180, 0); // Set width to 150 pixels, height to default
		ImGui::SetNextItemWidth(input_text_size.x);
		ImGui::InputText("##search_in_project_content_dir", search_query.data(), search_query.capacity(), ImGuiInputTextFlags_CharsNoBlank, NULL, NULL);
		ImGui::SameLine();
		if (ImGui::Button("search")) {

			CORE_LOG(Warn, "NOT INPLEMENTED YET");
		}
		ImGui::SameLine();

	}



	void content_browser::select_new_directory(const std::filesystem::path& path) {

		m_selected_directory = path;
		m_partial_selected_directory = extract_from_folder(m_selected_directory);
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

				if (buffer) {

					show_directory_tree(entry.path());
					ImGui::TreePop();
				}

			} else {

				ImGui::TreeNodeEx(entry.path().filename().string().c_str(), base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					select_new_directory(entry.path());

			}
		}

	}

	void content_browser::show_current_folder_content(const std::filesystem::path& path) {

		ImGuiStyle& style = ImGui::GetStyle();
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		ImVec2 button_sz = m_icon_size; // Use the size of the icon for the button
		int count = 0;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (!entry.is_directory())
				continue;

			// Begin a new group for each item
			ImGui::BeginGroup();
			{
				ImGui::PushID(count++);

				if (entry.is_directory()) {
					ImGui::Image(m_folder_image->get_descriptor_set(), m_icon_size);
				} else if (entry.path().extension() == PFF_ASSET_EXTENTION) {
					ImGui::Image(m_mesh_asset_image->get_descriptor_set(), m_icon_size);
				}

				// Calculate text size and position
				const std::string item_name = entry.path().filename().string();
				ImVec2 text_size = ImGui::CalcTextSize(item_name.c_str());
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((m_icon_size.x - text_size.x) * 0.5f, 0));
				ImGui::Text(item_name.c_str());

				ImGui::PopID();
			}
			ImGui::EndGroup();

			// Handle mouse click for item selection
			if (ImGui::IsItemClicked()) {
				select_new_directory(entry.path());
			}

			// Handle item wrapping
			float last_item_x2 = ImGui::GetItemRectMax().x;
			float next_item_x2 = last_item_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next item was on the same line

			if (next_item_x2 < window_visible_x2) {
				ImGui::SameLine();
			}
		}


		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (entry.is_directory())
				continue;


			// Begin a new group for each item
			ImGui::BeginGroup();
			{
				ImVec2 item_pos = ImGui::GetCursorScreenPos();
				ImGui::Image(m_mesh_asset_image->get_descriptor_set(), m_icon_size);
				ImVec2 item_pos_end = ImGui::GetCursorScreenPos();

				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				draw_list->AddRectFilled(item_pos, item_pos + m_icon_size, background_color, 4);

				ImGui::PushID(count++);

				const std::string item_name = entry.path().filename().string();
				ImVec2 text_size = ImGui::CalcTextSize(item_name.c_str());
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((m_icon_size.x - text_size.x) * 0.5f, 0));
				ImGui::Text(item_name.c_str());

				ImGui::PopID();
			}
			ImGui::EndGroup();

			// handle mouse click for item selection
			if (ImGui::IsItemClicked()) {


			}

			// handle item wrapping
			float last_item_x2 = ImGui::GetItemRectMax().x;
			float next_item_x2 = last_item_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next item was on the same line

			if (next_item_x2 < window_visible_x2) {
				ImGui::SameLine();
			}
		}

	}


	content_browser::content_browser() {

		m_project_directory = PFF_editor::get().get_project_path();
		select_new_directory(m_project_directory / CONTENT_DIR);
		window_name = "Content Browser";

		// load folder image
		m_folder_image = create_ref<image>(PFF_editor::get().get_editor_executable_path() / "assets" / "folder.png", image_format::RGBA);
		m_mesh_asset_image = create_ref<image>(PFF_editor::get().get_editor_executable_path() / "assets" / "mesh_asset.png", image_format::RGBA);

		m_icon_size = { 80, 80 };
	}

	content_browser::~content_browser() {}


	//struct TEST_STRUCT {

	//	std::filesystem::path		path;
	//};

	void content_browser::window() {

		if (!show_window)
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::Begin(window_name.c_str(), &show_window, window_flags);

		UI::custom_frame_NEW(350, true, IM_COL32(37, 37, 37, 255), [&]() {

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
			DisplaySearchFieldAndButtons(m_selected_directory);

			ImGui::SameLine();
			UI::shift_cursor_pos(10, 0);
			std::filesystem::path current_path;
			for (const auto& part : m_partial_selected_directory) {

				current_path /= part;
				if (UI::gray_button(part.string().c_str())) {

					select_new_directory(m_project_directory / current_path);
					break;
				}

				if (part != m_partial_selected_directory.filename()) {

					ImGui::SameLine();
					ImGui::Text("/");
					ImGui::SameLine();
				}
			}


			show_current_folder_content(m_selected_directory);

		});


		ImGui::End();
	}

}
