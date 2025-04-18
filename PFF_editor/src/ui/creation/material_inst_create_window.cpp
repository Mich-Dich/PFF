
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "resource_management/factories/texture_factory.h"
#include "PFF_editor.h"

#include "material_inst_create_window.h"

namespace PFF {


	material_inst_create_window::material_inst_create_window(const std::filesystem::path destination_path)
		: destination_path(destination_path) {

		m_asset_alredy_exists = true;													// default to true while in dev
		m_possible_asset_name = "new_instance";
		// m_metadata_available = material_factory::get_metadata(source_path, metadata);
	}

	void material_inst_create_window::window() {

		if (!show_window)
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		if (ImGui::Begin("Material Instance creator", &show_window, window_flags)) {

			ImGui::Text("Name of new material instance");
			if (ImGui::InputText("asset_name", &m_possible_asset_name, 256*sizeof(char)))
				m_asset_alredy_exists = std::filesystem::exists(destination_path / std::string(m_possible_asset_name));

			// UI::begin_table("texture_import_dialog", false);
			// m_source_string = source_path.string();
			// UI::table_row_text("Source", (m_source_string.c_str()));
			// UI::end_table();
			
			UI::shift_cursor_pos(0, 10);
			if (m_asset_alredy_exists) {

				ImGui::Image(PFF_editor::get().get_editor_layer()->get_warning_icon()->get_descriptor_set(), ImVec2(15), ImVec2(0, 0), ImVec2(1, 1), ImVec4(.9f, .5f, 0.f, 1.f));
				ImGui::SameLine();
				ImGui::TextWrapped("The asset your trying to import already exists, if you continue you are overwriting the previous asset");				
			}
			UI::shift_cursor_pos(0, 10);

			const f32 width = ImGui::GetContentRegionAvail().x / 2 - (ImGui::GetStyle().ItemSpacing.x * 2);
			if (ImGui::Button("Cancle", ImVec2(width, 0)))
				show_window = false;

			ImGui::SameLine();

			if (m_asset_alredy_exists)
				ImGui::BeginDisabled();
			if (ImGui::Button("Create", ImVec2(width, 0))) {
				
				texture_factory::import(source_path, destination_path, loc_load_options);
				show_window = false;
			}
			if (m_asset_alredy_exists)
				ImGui::EndDisabled();

		}
		ImGui::End();
	}

}
