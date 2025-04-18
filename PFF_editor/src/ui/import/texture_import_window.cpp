
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "resource_management/factories/texture_factory.h"
#include "PFF_editor.h"

#include "texture_import_window.h"

namespace PFF {

	static texture_factory::load_options loc_load_options{};								// should remenber import settings for this session		only one option setting for all sources
	static std::vector<std::pair<bool, texture_factory::texture_metadata>> all_metadata{};
	// static std::vector<texture_factory::texture_metadata> all_metadata{};

	texture_import_window::texture_import_window(const std::vector<std::filesystem::path>&& source_paths, const std::filesystem::path destination_path)
		: source_paths(source_paths), destination_path(destination_path) {

		if (source_paths.empty())
			show_window = false;

		m_number_of_sources = source_paths.size();
		for (const auto path : source_paths) {

			m_asset_alredy_exists = PFF::texture_factory::check_if_assets_already_exists(path, destination_path, loc_load_options, m_assets_that_already_exist);
			texture_factory::texture_metadata loc_metadata{};
			bool metadata_available = texture_factory::get_metadata(path, loc_metadata);
			all_metadata.emplace_back(metadata_available, loc_metadata);
		}
	}

	void texture_import_window::window() {

		if (!show_window)
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		if (ImGui::Begin("Texture Importer", &show_window, window_flags)) {

			// === METADATA SECTION ===
			ImGui::Text("Metadata");
			ImGui::Separator();

			for (size_t i = 0; i < source_paths.size(); ++i) {
				const std::string title = "Source: " + source_paths[i].filename().string();
				if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
					UI::begin_table(("meta_table_" + std::to_string(i)).c_str(), false);

					if (all_metadata[i].first) {
						const auto &m = all_metadata[i].second;
						UI::table_row_text("Path", "%s", source_paths[i].string().c_str());
						UI::table_row_text("Size", "%u x %u", m.width, m.height);
						UI::table_row_text("Format", "%s (Channels: %d)", m.format.c_str(), m.channels);
						if (loc_load_options.generate_mipmaps)
							UI::table_row_text("Mip Levels", "%zu", m.mip_levels);
					} else {
						UI::table_row_text("Info", "No metadata available");
					}

					UI::end_table();
					ImGui::Spacing();
				}
			}

			ImGui::Text("Settings");
			ImGui::Separator();
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

				if (m_number_of_sources == 1) 												// display of only one file
					texture_factory::import(source_paths[0], destination_path, loc_load_options);

				else
					for (const auto path : source_paths)
						texture_factory::import(path, destination_path, loc_load_options);
	
				show_window = false;
			}

		}
		ImGui::End();
	}

}
