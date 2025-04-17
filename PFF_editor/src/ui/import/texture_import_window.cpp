
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "resource_management/factories/texture_factory.h"
#include "PFF_editor.h"

#include "texture_import_window.h"

#define IGNORE_RECHECKING					// not needed here because a texture cant be combined like static_mesh
namespace PFF {

	static texture_factory::load_options loc_load_options{};								// should remenber import settings for this session
	static texture_factory::texture_metadata metadata{};
#ifndef IGNORE_RECHECKING
	static texture_factory::load_options loc_prev_load_options{};
#endif

	texture_import_window::texture_import_window(const std::filesystem::path source_path, const std::filesystem::path destination_path) : source_path(source_path), destination_path(destination_path) {

#ifndef IGNORE_RECHECKING
		loc_prev_load_options.combine_meshes = !loc_load_options.combine_meshes;					// make sure it checks the mesh_factory at startup
#else
		m_asset_alredy_exists = PFF::texture_factory::check_if_assets_already_exists(source_path, destination_path, loc_load_options, m_assets_that_already_exist);
		m_metadata_available = texture_factory::get_metadata(source_path, metadata);
#endif
	}

	void texture_import_window::window() {

		if (!show_window)
			return;

#ifndef IGNORE_RECHECKING
		if (loc_load_options != loc_prev_load_options) {		// updated, need to check factory
			
			m_asset_alredy_exists = PFF::texture_factory::check_if_assets_already_exists(source_path, destination_path, loc_load_options, m_assets_that_already_exist);
			loc_prev_load_options = loc_load_options;
		}
#endif

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		if (ImGui::Begin("Texture Importer", &show_window, window_flags)) {

			ImGui::Text("Metadata");
			UI::begin_table("texture_import_dialog", false);
			m_source_string = source_path.string();
			UI::table_row_text("Source", (m_source_string.c_str()));
			if (m_metadata_available) {
				UI::table_row_text("Size", "%d / %d", metadata.width, metadata.height);
				UI::table_row_text("Format", "chanels [%d] - format [%s]", metadata.channels, metadata.format.c_str());
				if (loc_load_options.generate_mipmaps)
				UI::table_row_text("Mip levels", "%lu", metadata.mip_levels);
			}
			UI::end_table();
			
			ImGui::Text("Settings");
			UI::begin_table("texture_import_dialog", false);
			UI::table_row("generate mipmaps", loc_load_options.generate_mipmaps);
			UI::table_row("compress texture", loc_load_options.compress_texture);
			UI::table_row("flip X", loc_load_options.flip_x);
			UI::table_row("flip Y", loc_load_options.flip_y);
			UI::end_table();

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
			if (ImGui::Button("Import", ImVec2(width, 0))) {

				texture_factory::import(source_path, destination_path, loc_load_options);
				show_window = false;
			}

		}
		ImGui::End();
	}

}
