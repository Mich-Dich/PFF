
#include "util/pffpch.h"

#include "application.h"
#include "engine/render/vulkan/vk_renderer.h"
#include "mesh_serializer.h"

#include "static_mesh_asset_manager.h"

/*	GUIDLINE:
		meshes can be requested by path relative to the project [content] folder
		uploaded meshes will be held alive with a ref<> from this manager		// manager will loop over refs periodicly and release them if count is 1 (only used here)
*/

namespace PFF {

	static_mesh_asset_manager static_mesh_asset_manager::s_instance;

	static_mesh_asset_manager::static_mesh_asset_manager() { }

	ref<geometry::mesh_asset> static_mesh_asset_manager::get_from_path(const std::filesystem::path path) {

		CORE_VALIDATE(path.extension() == PFF_ASSET_EXTENTION, return nullptr, "", "Provided path is not a PFF-asset");
		//return nullptr;		// NOT FINISHED YET

		std::filesystem::path mesh_path = application::get().get_project_path() / CONTENT_DIR / path;

		if (s_instance.m_uploaded_mesh_assets.find(mesh_path) != s_instance.m_uploaded_mesh_assets.end())
			return s_instance.m_uploaded_mesh_assets[mesh_path];

		else {

			asset_file_header asset_file_header{};
			general_mesh_file_header general_header{};
			static_mesh_file_header static_mesh_header{};

			s_instance.m_uploaded_mesh_assets[mesh_path] = create_ref<geometry::mesh_asset>();
			serialize_mesh(mesh_path, s_instance.m_uploaded_mesh_assets[mesh_path], asset_file_header, general_header, static_mesh_header, serializer::option::load_from_file);

			CORE_ASSERT(asset_file_header.type == file_type::mesh, "", "Tryed to load an asset file that is not a mesh");
			// TODO: validate deserialized headers [general_header] / [static_mesh_header]

			// upload mesh to GPU
			s_instance.m_uploaded_mesh_assets[mesh_path]->mesh_buffers = GET_RENDERER.upload_mesh(s_instance.m_uploaded_mesh_assets[mesh_path]->indices, s_instance.m_uploaded_mesh_assets[mesh_path]->vertices);

			return s_instance.m_uploaded_mesh_assets[mesh_path];
		}

		return nullptr;
	}

	// TODO: ON SUB_THREAD => Iterate over every ref and check for refrence_count
	//			if the count is 1 it means only the asset manager is holding onto the asset and it should be released

}



// MAYBE:
		// loop over content directory to load a map of all static_mesh_assets	(only the map)
			//register_all_static_mesh_assets(application::get().get_project_path() / CONTENT_DIR);



/* TODO: Move to a terrain generation script

// ================================================ TERRAIN GENERATOR ===============================================
// procedural terrain test

//#define PROFILE_GENERATION
//
//	geometry::mesh_asset loc_mesh{};
//	loc_mesh.name = "procedural_test";
//
//#ifdef PROFILE_GENERATION
//	for (size_t i = 0; i < 10; i++) {
//		loc_mesh = geometry::mesh{};		// reset
//		CORE_LOG(Trace, "starting mesh generation iteration: " << i);
//		PFF_PROFILE_SCOPE("Procedural Grid Mesh");
//#endif
//
//		PFF::util::noise noise(PFF::util::noise_type::perlin);
//		noise.Set_fractal_type(PFF::util::fractal_type::FBm);
//		noise.set_frequency(0.005f);
//		noise.set_fractal_octaves(4);
//		noise.set_fractal_lacunarity(2.3f);
//
//		const glm::ivec2 grid_size = glm::ivec2(5);			// number of grid tiles
//		const glm::vec2 grid_tile_size = glm::ivec2(1000);	// size of a grid tile
//		const glm::vec2 grid_resolution = glm::ivec2(100);
//		const int iterations_x = static_cast<u32>(grid_size.x * grid_resolution.x);
//		const int iterations_y = static_cast<u32>(grid_size.y * grid_resolution.y);
//
//		const glm::vec2 offset = glm::vec2(
//			(static_cast<f32>(grid_size.x) / 2) * grid_tile_size.x,
//			(static_cast<f32>(grid_size.y) / 2) * grid_tile_size.y
//		);
//		const glm::vec2 pos_multiplier = glm::vec2(
//			(grid_tile_size.x / grid_resolution.x),
//			(grid_tile_size.y / grid_resolution.y)
//		);
//
//		{
//#ifdef PROFILE_GENERATION
//			PFF_PROFILE_SCOPE("Procedural Grid Mesh - resize vectors");
//#endif
//			loc_mesh.vertices.resize((iterations_x + 1) * (iterations_y + 1));
//			loc_mesh.indices.resize((iterations_x * iterations_y) * 6);
//		}
//
//		{
//#ifdef PROFILE_GENERATION
//			PFF_PROFILE_SCOPE("Procedural Grid Mesh - generate vertexes");
//#endif
//			int counter_vert = 0;
//			for (int y = 0; y <= iterations_y; ++y) {
//				for (int x = 0; x <= iterations_x; ++x) {
//
//					loc_mesh.vertices[counter_vert] = geometry::vertex(
//						glm::vec3(
//						(x * pos_multiplier.x) - offset.x,
//						noise.get_noise((f32)x, (f32)y) * 500.f,
//						(y * pos_multiplier.y) - offset.y
//					),
//						{ 0, 0, 1 },
//						glm::vec4{ 1.f },
//						static_cast<f32>(x) / grid_resolution.x,
//						static_cast<f32>(y) / grid_resolution.y
//					);
//					counter_vert++;
//				}
//			}
//		}
//
//		{
//#ifdef PROFILE_GENERATION
//			PFF_PROFILE_SCOPE("Procedural Grid Mesh - generate triange");
//#endif
//			int counter = 0;
//			for (int y = 0; y < iterations_y; y++) {
//				for (int x = 0; x < iterations_x; x++) {
//
//					int top_left = y * ((iterations_x)+1) + x;
//					int top_right = top_left + 1;
//					int bottom_left = (y + 1) * ((iterations_x)+1) + x;
//					int bottom_right = bottom_left + 1;
//
//					loc_mesh.indices[counter + 0] = top_left;
//					loc_mesh.indices[counter + 1] = bottom_left;
//					loc_mesh.indices[counter + 2] = top_right;
//
//					loc_mesh.indices[counter + 3] = top_right;
//					loc_mesh.indices[counter + 4] = bottom_left;
//					loc_mesh.indices[counter + 5] = bottom_right;
//					counter += 6;
//				}
//			}
//		}
//
//		geometry::Geo_surface new_surface{};
//		new_surface.count = static_cast<int>(loc_mesh.indices.size());
//		loc_mesh.surfaces.push_back(new_surface);
//
//#ifdef PROFILE_GENERATION
//	}
//#endif
//
//	T_test_meshes.emplace_back(create_ref<geometry::mesh_asset>(std::move(loc_mesh)));
//
//	if (T_test_meshes.size() > 0) {
//
//		for (auto mesh : T_test_meshes)
//			mesh->mesh_buffers = upload_mesh(mesh->indices, mesh->vertices);
//	}
*/