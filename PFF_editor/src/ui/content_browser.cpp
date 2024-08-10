
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "mesh_import_window.h"
#include "PFF_editor.h"

#include "content_browser.h"

namespace PFF {

	static bool show_import_window = false;
	static const std::filesystem::path project_directory = std::filesystem::path("C:\\") / "CustomGameEngine" / "PFF" / "test_project";
	static std::filesystem::path selected_directory = std::filesystem::path("");


	content_browser::content_browser() { window_name = "Content Browser"; }


	FORCEINLINE static void show_directory_tree(const std::filesystem::path& path) {

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			bool has_sub_folders = false;
			for (const auto& sub_entry : std::filesystem::directory_iterator(entry.path())) {
				if (sub_entry.is_directory()) {
					has_sub_folders = true;
					break;
				}
			}

			if (has_sub_folders) {

				bool buffer = ImGui::TreeNodeEx(entry.path().filename().string().c_str(), base_flags);
				if (ImGui::IsItemClicked())
					selected_directory = entry.path();

				if (buffer) {

					show_directory_tree(entry.path());
					ImGui::TreePop();
				}

			} else {

				ImGui::TreeNodeEx(entry.path().filename().string().c_str(), base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					selected_directory = entry.path();

			}
		}

    }


	void content_browser::window() {

		if (!show_window)
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::Begin(window_name.c_str(), &show_window, window_flags);

		if (ImGui::Button(" import ##content_browser_import")) {

			std::filesystem::path source_path = util::file_dialog();
			static_cast<PFF_editor&>(application::get()).get_editor_layer()->add_window<mesh_import_window>( source_path, selected_directory );
		}
		
		UI::custom_frame_NEW(350, true, IM_COL32(37, 37, 37, 255), []() {
			
			show_directory_tree(project_directory / "content");

		}, []() {


		});


		ImGui::End();
	}

}
