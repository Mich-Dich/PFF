
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
		m_asset_progress.assign(m_number_of_sources, 0.f);
		for (const auto path : source_paths) {

			m_asset_alredy_exists = PFF::texture_factory::check_if_assets_already_exists(path, destination_path, loc_load_options, m_assets_that_already_exist);
			texture_factory::texture_metadata loc_metadata{};
			bool metadata_available = texture_factory::get_metadata(path, loc_metadata);
			all_metadata.emplace_back(metadata_available, loc_metadata);
		}
	}

	void texture_import_window::start_import() {
		m_importing = true;
		m_current_index = 0;
	
		for (size_t i = 0; i < source_paths.size(); ++i) {
			m_current_index = i;
			// Reset per-asset
			m_asset_progress[i] = 0.f;
	
			// Call import with a lambda to update per-asset progress.
			texture_factory::import(source_paths[i], destination_path, loc_load_options,
				[&](f32 frac) {					// Progress callback: fraction from 0 to 1
					m_asset_progress[i] = frac;
				}
			);
	
			// Ensure fully complete
			m_asset_progress[i] = 1.f;
		}
	
		m_importing = false;
		show_window = false;
	}
	
	void texture_import_window::window() {

		if (!show_window)
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Appearing);
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

				if (m_importing) {

					const f32 avail = ImGui::GetContentRegionAvail().x;
					const f32 bar_w = avail * 0.8f;
					UI::shift_cursor_pos(avail * 0.1f, 0);
					const std::string progress = std::to_string(int(m_asset_progress[i] * 100));
					ImGui::ProgressBar(m_asset_progress[i], ImVec2(bar_w, 0), progress.c_str());
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


			if (m_importing) {

				UI::shift_cursor_pos(0, 10);
				ImGui::Separator();
				ImGui::Text("Import Progress:");
			
				// Overall progress: average of assets
				f32 overall = 0.f;
				for (auto p : m_asset_progress)
					overall += p;
				overall /= f32(m_number_of_sources);
				
				const std::string progress_text = "Overall " + std::to_string(int(overall * 100)) + "%";
				ImGui::ProgressBar(overall, ImVec2(-1, 0), progress_text.c_str());
			}
		
			ImGui::Spacing();
			ImGui::Separator();
	
			// Buttons
			ImGui::BeginDisabled(m_importing);
			const f32 width = ImGui::GetContentRegionAvail().x / 2 - (ImGui::GetStyle().ItemSpacing.x * 2);
			if (ImGui::Button("Cancel", ImVec2(width, 0))) {
				show_window = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Import", ImVec2(width, 0))) {
				// Launch worker thread
				if (!m_importing) {
					m_import_thread = std::thread(&texture_import_window::start_import, this);
					m_import_thread.detach();
				}
			}
			ImGui::EndDisabled();
		}
		ImGui::End();
	}

}
