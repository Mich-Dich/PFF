#pragma once

#include "headers.h"
#include "mesh_headers.h"

#include "engine/geometry/mesh.h"

namespace PFF {

	PFF_API void serialize_mesh(const std::filesystem::path filename, ref<geometry::mesh_asset> mesh_asset, 
		asset_file_header& asset_file_header, general_mesh_file_header& general_header, static_mesh_file_header& static_mesh_header, const serializer::option option);

	class PFF_API static_mesh_asset_manager {
	public:

		static static_mesh_asset_manager& get() { return s_instance; }

		// @breif path needs to be relative to project_dir/content
		static ref<geometry::mesh_asset> get_from_path(const std::filesystem::path path);

	private:

		static_mesh_asset_manager();
		static static_mesh_asset_manager s_instance;
		
		//FORCEINLINE void register_all_static_mesh_assets(const std::filesystem::path& path);

		std::unordered_map<std::filesystem::path, ref<geometry::mesh_asset>>		m_uploaded_mesh_assets;
		std::vector<ref<PFF::geometry::mesh_asset>>									T_test_meshes;

	};

}
