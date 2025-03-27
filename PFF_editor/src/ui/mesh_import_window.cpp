
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "resource_management/factories/static_mesh_factory.h"

#include "mesh_import_window.h"

namespace PFF {


	static std::filesystem::path project_dir = std::filesystem::path("C:\\") / "CustomGameEngine" / "PFF" / "test_project" / CONTENT_DIR;


	void mesh_import_window::window() {

		if (!show_window)
			return;
		
		static mesh_factory::load_options loc_load_options{};



		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::Begin("mesh importer", &show_window, window_flags)) {

			//ImGui::Text("Source: %s", (source_path.string().c_str()));


			UI::begin_table("Mesh Import Settings");

			UI::table_row_text("Source", (source_path.string().c_str()));
			UI::table_row("combine meshes", loc_load_options.combine_meshes);
			UI::table_row("auto generate LODs", loc_load_options.auto_generate_LODs);
			
			UI::end_table();


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
