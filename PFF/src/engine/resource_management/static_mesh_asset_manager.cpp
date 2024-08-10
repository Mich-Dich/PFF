
#include "util/pffpch.h"

#include "static_mesh_asset_manager.h"

namespace PFF {

	static_mesh_asset_manager static_mesh_asset_manager::s_instance;

	static_mesh_asset_manager::static_mesh_asset_manager() {
	
		// TODO: loop over content directory to load a map of all static_mesh_assets	(only the map)
		

		// T_test_meshes = PFF::mesh_factory::load_gltf_meshes("../PFF/assets/meshes/grass_001.glb").value();



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

	//template<typename T>
	//static bool cointains()

	ref<geometry::mesh_asset> static_mesh_asset_manager::get_from_path(const std::filesystem::path path) {

		CORE_VALIDATE(path.extension() == ".sm", return nullptr, "", "Provided path is not a static_mesh");




		return nullptr;		// NOT FINISHED YET




		if (s_instance.m_mesh_asset_map.find(path) == s_instance.m_mesh_asset_map.end())
			return s_instance.m_mesh_asset_map[path];

		else {

			// TODO:: load from disk	=> shoudld be in engine format
			//geometry::mesh_asset loc_mesh = load_from_disk(path);
						
			//s_instance.m_mesh_asset_map[path] = create_ref<geometry::mesh_asset>(loc_mesh);				// save pointer in map
			return s_instance.m_mesh_asset_map[path];
		}

		return nullptr;
	}

	// TODO: Iterate over every ref and check for refrence_count
	//			if the count is 1 it means only the asset manager is holding onto the asset and it should be released

}


// H:
//
//// ==========================================================================================================
//// !!!!!!!!!!!!!!!! DEV-ONLY !!!!!!!!!!!!!!!!!!!!!!
//ref<PFF::geometry::mesh_asset>	get_test_mesh() const { return T_test_meshes[0]; }
//PFF_DEFAULT_GETTERS_C(material_instance, default_material);
//// ==========================================================================================================

