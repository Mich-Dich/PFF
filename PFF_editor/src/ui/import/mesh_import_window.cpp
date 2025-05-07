
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "resource_management/factories/static_mesh_factory.h"
#include "PFF_editor.h"

#include "mesh_import_window.h"

namespace PFF {

	static mesh_factory::load_options loc_load_options{};										// should remenber import settings for this session
	static mesh_factory::load_options loc_prev_load_options{};
	static std::vector<std::pair<bool, mesh_factory::metadata>> all_metadata{};

	mesh_import_window::mesh_import_window(const std::vector<std::filesystem::path>&& source_paths, const std::filesystem::path destination_path)
		: source_paths(source_paths), destination_path(destination_path) {

		LOG(Trace, "Opening import dialog");
		loc_prev_load_options.combine_meshes = !loc_load_options.combine_meshes;					// make sure it checks the mesh_factory at startup
        m_import_threads.reserve(source_paths.size());
		check_if_assets_exist();
		LOG(Trace, "mesh_import_window init");
	}

	mesh_import_window::~mesh_import_window() {
		// If manager thread exists and is joinable, wait for it
		if (m_import_manager && m_import_manager->joinable())
			m_import_manager->join();
	
		// ... other cleanup ...
	}
	
	void mesh_import_window::check_if_assets_exist() {

		for (const auto path : source_paths) {
	
			m_asset_alredy_exists = mesh_factory::check_if_assets_already_exist(path, destination_path, loc_load_options, m_assets_that_already_exist);
			mesh_factory::metadata loc_metadata{};
			bool metadata_available = mesh_factory::get_metadata(path, loc_metadata);
			all_metadata.emplace_back(metadata_available, loc_metadata);
		}
		loc_prev_load_options = loc_load_options;
	}

	void mesh_import_window::start_import() {
		
		LOG(Debug, "start import proccess");
		m_importing = true;
		for (size_t i = 0; i < source_paths.size(); ++i) {

			m_import_threads.emplace_back();								// Default-construct the struct
            auto& thread_struct = m_import_threads.back();
            thread_struct.thread = std::thread(&PFF::mesh_factory::import_mesh, source_paths[i], destination_path, loc_load_options, std::ref(thread_struct.progress) );
		}

		LOG(Debug, "Waiting for thread to finish");

		for (auto& [progress, thread] : m_import_threads)
			if (thread.joinable())
				thread.join();
	
		LOG(Debug, "import finished");
        m_import_threads.clear();
		m_importing = false;
		show_window = false;
	}

	void mesh_import_window::window() {

		if (!show_window)
			return;

		if (loc_load_options != loc_prev_load_options)			// updated, need to check factory
			check_if_assets_exist();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Appearing);
		if (ImGui::Begin("Mesh Importer", &show_window, window_flags)) {

			// === METADATA SECTION ===
			ImGui::Text("Metadata");
			ImGui::Separator();
			
			for (size_t i = 0; i < source_paths.size(); ++i) {
				const std::string title = "Source: " + source_paths[i].filename().string();
				if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
					UI::begin_table(("meta_table_" + std::to_string(i)).c_str(), false);

					if (all_metadata[i].first) {
						const mesh_factory::metadata &loc_metadata = all_metadata[i].second;
						UI::table_row_text("File Format", 			"%s", loc_metadata.file_format.c_str());
						UI::table_row_text("File Size", 			"%u MB", loc_metadata.file_size);
						UI::table_row_text("Number of meshes", 		"%u", loc_metadata.num_meshes);
						UI::table_row_text("Total Vertices", 		"%u", loc_metadata.total_vertices);
						UI::table_row_text("Total Faces", 			"%u", loc_metadata.total_faces);
						UI::table_row_text("Has Normals", 			"%s", util::to_string(loc_metadata.has_normals).c_str());
						UI::table_row_text("Has UVs", 				"%s", util::to_string(loc_metadata.has_uvs).c_str());
						UI::table_row_text("Has Vertex Colors", 	"%s", util::to_string(loc_metadata.has_vertex_colors).c_str());
						UI::table_row_text("Has Bones", 			"%s", util::to_string(loc_metadata.has_bones).c_str());
						UI::table_row_text("Number of  Materials", 	"%u", loc_metadata.num_materials);
						UI::table_row_text("Number of Animations", 	"%u", loc_metadata.num_animations);
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
					const std::string progress = std::to_string(int(m_import_threads[i].progress * 100));
					ImGui::ProgressBar(m_import_threads[i].progress, ImVec2(bar_w, 0), progress.c_str());
				}
			}

			ImGui::Text("Settings");
			ImGui::Separator();

			if (m_importing)
				ImGui::BeginDisabled();

			UI::begin_table("mesh_import_dialog_settings", false);
			UI::table_row("combine meshes", 			loc_load_options.combine_meshes);
			if (!loc_load_options.combine_meshes)
				UI::table_row("include file name in asset name", loc_load_options.include_file_name_in_asset_name);

			UI::table_row("auto generate LODs", 		loc_load_options.auto_generate_LODs);
			UI::table_row("Ggenerate smooth normals", 	loc_load_options.generate_smooth_normals);
			UI::table_row("Triangulate", 				loc_load_options.triangulate);
			UI::table_row("Flip UVs", 					loc_load_options.flipUVs);
			UI::table_row("Join identical vertices", 	loc_load_options.join_identical_vertices);
			
			UI::end_table();

			UI::shift_cursor_pos(0, 10);
			if (m_asset_alredy_exists) {

				ImGui::Image(PFF_editor::get().get_editor_layer()->get_warning_icon()->get_descriptor_set(), ImVec2(15), ImVec2(0, 0), ImVec2(1, 1), ImVec4(.9f, .5f, 0.f, 1.f));
				ImGui::SameLine();

				if (loc_load_options.combine_meshes)
					ImGui::Text("The asset your trying to import already exists.\nIf you continue you are overwriting the previous asset");
				else {

					if (m_assets_that_already_exist.size() == 1)
						ImGui::Text("An asset your trying to import already exists, if you continue you are overwriting the previous asset");
					else
						ImGui::Text("Multiple assets your trying to import already exist, if you continue you are overwriting the previous assets");

					ImGui::Text("Assets that will be overwriten in current directory:");
					
					UI::shift_cursor_pos(25, 0);
 					ImVec2 childSize = ImVec2(ImGui::GetContentRegionAvail().x - 50, 150); // full width, 150 pixels tall (for example)
					ImGui::BeginChild("ScrollableNamesSection", childSize, true);
						for (const auto name : m_assets_that_already_exist)
							ImGui::TextWrapped("%s", name.c_str());
					ImGui::EndChild();
				}
				
			}
			UI::shift_cursor_pos(0, 10);

			if (m_importing)
				ImGui::EndDisabled();
			
			const f32 width = ImGui::GetContentRegionAvail().x / 2 - (ImGui::GetStyle().ItemSpacing.x * 2);
			if (ImGui::Button("Cancle", ImVec2(width, 0)))
				show_window = false;

			ImGui::SameLine();
			if (ImGui::Button("Import", ImVec2(width, 0))) {
				if (!m_importing) {

					m_import_manager = std::make_unique<std::thread>([this]() { start_import(); });							// Launch thread to manage imports
				}
			}

		}
		ImGui::End();
	}

}
