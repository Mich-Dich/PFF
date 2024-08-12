
#include "util/pffpch.h"

#include "application.h"
#include "engine/render/vulkan/vk_renderer.h"

#include "static_mesh_asset_manager.h"

namespace PFF {


	/*	GUIDLINE:
			meshes can be requested by path relative to the project content folder
			uploaded meshes will be heald alive with a ref<> from this manager		// manager will loop over refs periodicly and release them if count is 1 (only used here)

	*/


	static_mesh_asset_manager static_mesh_asset_manager::s_instance;

	// start with PFF_editor::get().get_project_dir() / CONTENT_DIR
	//void static_mesh_asset_manager::register_all_static_mesh_assets(const std::filesystem::path& path) {

	//	m_uploaded_mesh_assets["sfsgdfg"] = nullptr;

	//	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
	//	for (const auto& entry : std::filesystem::directory_iterator(path)) {

	//		if (!entry.is_directory())
	//			continue;

	//		bool has_sub_folders = false;
	//		for (const auto& sub_entry : std::filesystem::directory_iterator(entry.path())) {		// <= HERE I GET AN ERROR
	//			if (!sub_entry.is_directory())
	//				continue;

	//			has_sub_folders = true;
	//			break;
	//		}

	//		if (has_sub_folders) {

	//			bool buffer = ImGui::TreeNodeEx(entry.path().filename().string().c_str(), base_flags);
	//			if (ImGui::IsItemClicked())
	//				m_selected_directory = entry.path();

	//			if (buffer) {

	//				show_directory_tree(entry.path());
	//				ImGui::TreePop();
	//			}

	//		} else {

	//			ImGui::TreeNodeEx(entry.path().filename().string().c_str(), base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
	//			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	//				m_selected_directory = entry.path();

	//		}
	//	}

	//}

	static_mesh_asset_manager::static_mesh_asset_manager() {
	
		// TODO: loop over content directory to load a map of all static_mesh_assets	(only the map)

		//register_all_static_mesh_assets(application::get().get_project_path() / CONTENT_DIR);


		




//#define MESH_SOURCE 1
//#if MESH_SOURCE == 0
//
//		T_test_meshes = IO::mesh_loader::load_gltf_meshes("../PFF/assets/meshes/basicmesh.glb").value();
//
//#elif MESH_SOURCE == 1
//
//		T_test_meshes = PFF::mesh_factory::load_gltf_meshes("../PFF/assets/meshes/grass_001.glb").value();
//
//#elif MESH_SOURCE == 2
//
//		T_test_meshes = IO::mesh_loader::load_gltf_meshes("../PFF/assets/meshes/BP-688.glb").value();
//
//#elif MESH_SOURCE == 3 	// procedural terrain test
//
////#define PROFILE_GENERATION
//
//		geometry::mesh_asset loc_mesh{};
//		loc_mesh.name = "procedural_test";
//
//#ifdef PROFILE_GENERATION
//		for (size_t i = 0; i < 10; i++) {
//			loc_mesh = geometry::mesh{};		// reset
//			CORE_LOG(Trace, "starting mesh generation iteration: " << i);
//			PFF_PROFILE_SCOPE("Procedural Grid Mesh");
//#endif
//
//			PFF::util::noise noise(PFF::util::noise_type::perlin);
//			noise.Set_fractal_type(PFF::util::fractal_type::FBm);
//			noise.set_frequency(0.005f);
//			noise.set_fractal_octaves(4);
//			noise.set_fractal_lacunarity(2.3f);
//
//			const glm::ivec2 grid_size = glm::ivec2(5);			// number of grid tiles
//			const glm::vec2 grid_tile_size = glm::ivec2(1000);	// size of a grid tile
//			const glm::vec2 grid_resolution = glm::ivec2(100);
//			const int iterations_x = static_cast<u32>(grid_size.x * grid_resolution.x);
//			const int iterations_y = static_cast<u32>(grid_size.y * grid_resolution.y);
//
//			const glm::vec2 offset = glm::vec2(
//				(static_cast<f32>(grid_size.x) / 2) * grid_tile_size.x,
//				(static_cast<f32>(grid_size.y) / 2) * grid_tile_size.y
//			);
//			const glm::vec2 pos_multiplier = glm::vec2(
//				(grid_tile_size.x / grid_resolution.x),
//				(grid_tile_size.y / grid_resolution.y)
//			);
//
//			{
//#ifdef PROFILE_GENERATION
//				PFF_PROFILE_SCOPE("Procedural Grid Mesh - resize vectors");
//#endif
//				loc_mesh.vertices.resize((iterations_x + 1) * (iterations_y + 1));
//				loc_mesh.indices.resize((iterations_x * iterations_y) * 6);
//			}
//
//			{
//#ifdef PROFILE_GENERATION
//				PFF_PROFILE_SCOPE("Procedural Grid Mesh - generate vertexes");
//#endif
//				int counter_vert = 0;
//				for (int y = 0; y <= iterations_y; ++y) {
//					for (int x = 0; x <= iterations_x; ++x) {
//
//						loc_mesh.vertices[counter_vert] = geometry::vertex(
//							glm::vec3(
//							(x * pos_multiplier.x) - offset.x,
//							noise.get_noise((f32)x, (f32)y) * 500.f,
//							(y * pos_multiplier.y) - offset.y
//						),
//							{ 0, 0, 1 },
//							glm::vec4{ 1.f },
//							static_cast<f32>(x) / grid_resolution.x,
//							static_cast<f32>(y) / grid_resolution.y
//						);
//						counter_vert++;
//					}
//				}
//			}
//
//			{
//#ifdef PROFILE_GENERATION
//				PFF_PROFILE_SCOPE("Procedural Grid Mesh - generate triange");
//#endif
//				int counter = 0;
//				for (int y = 0; y < iterations_y; y++) {
//					for (int x = 0; x < iterations_x; x++) {
//
//						int top_left = y * ((iterations_x)+1) + x;
//						int top_right = top_left + 1;
//						int bottom_left = (y + 1) * ((iterations_x)+1) + x;
//						int bottom_right = bottom_left + 1;
//
//						loc_mesh.indices[counter + 0] = top_left;
//						loc_mesh.indices[counter + 1] = bottom_left;
//						loc_mesh.indices[counter + 2] = top_right;
//
//						loc_mesh.indices[counter + 3] = top_right;
//						loc_mesh.indices[counter + 4] = bottom_left;
//						loc_mesh.indices[counter + 5] = bottom_right;
//						counter += 6;
//					}
//				}
//			}
//
//			geometry::Geo_surface new_surface{};
//			new_surface.count = static_cast<int>(loc_mesh.indices.size());
//			loc_mesh.surfaces.push_back(new_surface);
//
//#ifdef PROFILE_GENERATION
//		}
//#endif
//
//		T_test_meshes.emplace_back(create_ref<geometry::mesh_asset>(std::move(loc_mesh)));
//
//#endif
//
//		if (T_test_meshes.size() > 0) {
//
//			for (auto mesh : T_test_meshes)
//				mesh->mesh_buffers = upload_mesh(mesh->indices, mesh->vertices);
//		}

	}


	ref<geometry::mesh_asset> static_mesh_asset_manager::get_from_path(const std::filesystem::path path) {

		CORE_VALIDATE(path.extension() == PFF_ASSET_EXTENTION, return nullptr, "", "Provided path is not a static mesh asset");
		//return nullptr;		// NOT FINISHED YET

		std::filesystem::path mesh_path = application::get().get_project_path() / CONTENT_DIR / path;

		if (s_instance.m_uploaded_mesh_assets.find(mesh_path) != s_instance.m_uploaded_mesh_assets.end())
			return s_instance.m_uploaded_mesh_assets[mesh_path];

		else {

			general_file_header general_header{};
			static_mesh_header static_mesh_header{};

			s_instance.m_uploaded_mesh_assets[mesh_path] = create_ref<geometry::mesh_asset>();
			serialize_mesh(mesh_path, s_instance.m_uploaded_mesh_assets[mesh_path], general_header, static_mesh_header, serializer::option::load_from_file);

			// TODO: validate deserialized headers
			CORE_LOG(Info, "Deserialized mesh => general_header: " << (u64)general_header.type);
			CORE_LOG(Info, "Deserialized mesh => static_mesh_header: " << static_mesh_header.version
				<< " | "<< static_mesh_header.source_file
				<< " | "<< static_mesh_header.mesh_index);

			// upload mesh to GPU
			s_instance.m_uploaded_mesh_assets[mesh_path]->mesh_buffers = GET_RENDERER.upload_mesh(s_instance.m_uploaded_mesh_assets[mesh_path]->indices, s_instance.m_uploaded_mesh_assets[mesh_path]->vertices);

			return s_instance.m_uploaded_mesh_assets[mesh_path];
		}

		return nullptr;
	}

	// TODO: Iterate over every ref and check for refrence_count
	//			if the count is 1 it means only the asset manager is holding onto the asset and it should be released

	void serialize_mesh(const std::filesystem::path filename, ref<geometry::mesh_asset> mesh_asset, general_file_header& general_header, static_mesh_header& static_mesh_header, const serializer::option option) {

			serializer::binary(filename, "gltf_mesh_data", option)
				.entry(general_header)
				.entry(static_mesh_header.version)
				.entry(static_mesh_header.source_file)
				.entry(static_mesh_header.mesh_index)
				.entry(mesh_asset->surfaces)
				.entry(mesh_asset->vertices)
				.entry(mesh_asset->indices)
				.entry(mesh_asset->bounds);

	}

}


// H:
//
//// ==========================================================================================================
//// !!!!!!!!!!!!!!!! DEV-ONLY !!!!!!!!!!!!!!!!!!!!!!
//ref<PFF::geometry::mesh_asset>	get_test_mesh() const { return T_test_meshes[0]; }
//PFF_DEFAULT_GETTERS_C(material_instance, default_material);
//// ==========================================================================================================
