
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "resource_management/factories/static_mesh_factory.h"
#include "PFF_editor.h"

#include "mesh_import_window.h"

namespace PFF {

	// static std::filesystem::path project_dir = std::filesystem::path("C:\\") / "CustomGameEngine" / "PFF" / "test_project" / CONTENT_DIR;


	// mesh_import_window::mesh_import_window(const std::filesystem::path source_path, const std::filesystem::path destination_path) : source_path(source_path), destination_path(destination_path) {

	// 	asset_alredy_exists = 
	// }


	void mesh_import_window::window() {

		if (!show_window)
			return;
		
		static mesh_factory::load_options loc_load_options{};							// should remenber import settings for this session

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::Begin("mesh importer", &show_window, window_flags)) {

			UI::begin_table("Mesh Import Settings");

			UI::table_row_text("Source", (source_path.string().c_str()));
			UI::table_row("combine meshes", loc_load_options.combine_meshes);
			if (!loc_load_options.combine_meshes)
				UI::table_row("include file name in asset name", loc_load_options.include_file_name_in_asset_name);
			UI::table_row("auto generate LODs", loc_load_options.auto_generate_LODs);
			
			UI::end_table();

			UI::shift_cursor_pos(0, 10);
			if (PFF::mesh_factory::check_if_assets_already_exists(source_path, destination_path, loc_load_options, assets_that_already_exist)) {

				ImGui::Image(PFF_editor::get().get_editor_layer()->get_warning_icon()->get_descriptor_set(), ImVec2(15), ImVec2(0, 0), ImVec2(1, 1), ImVec4(.9f, .5f, 0.f, 1.f));
				ImGui::SameLine();

				if (loc_load_options.combine_meshes)
					ImGui::TextWrapped("The asset your trying to import already exists, if you continue you are overwriting the previous asset");
				else {

					if (assets_that_already_exist.size() == 1)
						ImGui::TextWrapped("An asset your trying to import already exists, if you continue you are overwriting the previous asset");
					else
						ImGui::TextWrapped("Multiple assets your trying to import already exist, if you continue you are overwriting the previous assets");

					ImGui::TextWrapped("Assets that will be overwriten in current directory:");
					ImGui::Indent();
					for (const auto name : assets_that_already_exist)
						ImGui::TextWrapped("%s", name.c_str());

					ImGui::Unindent();
				}

				
			}
			UI::shift_cursor_pos(0, 10);

			const f32 width = ImGui::GetContentRegionAvail().x / 2 - (ImGui::GetStyle().ItemSpacing.x * 2);
			if (ImGui::Button("Cancle", ImVec2(width, 0)))
				show_window = false;

			ImGui::SameLine();
			if (ImGui::Button("Import", ImVec2(width, 0))) {

				PFF::mesh_factory::import_gltf_mesh(source_path, destination_path, loc_load_options);
				show_window = false;
			}

		}
		ImGui::End();
	}

}
