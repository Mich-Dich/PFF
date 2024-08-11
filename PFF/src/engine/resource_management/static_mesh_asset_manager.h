#pragma once

#include "mesh_headers.h"

#include "engine/geometry/mesh.h"

namespace PFF {

	PFF_API void serialize_mesh(const std::filesystem::path filename, ref<geometry::mesh_asset> mesh_asset, general_file_header& general_header, static_mesh_header& static_mesh_header, const serializer::option option);

	class static_mesh_asset_manager {
	public:

		static static_mesh_asset_manager& get() { return s_instance; }

		static ref<geometry::mesh_asset> get_from_path(const std::filesystem::path path);

	private:
		static_mesh_asset_manager();
		static static_mesh_asset_manager s_instance;

		std::unordered_map<std::filesystem::path, ref<geometry::mesh_asset>>		m_mesh_asset_map;

		std::vector<ref<PFF::geometry::mesh_asset>>	T_test_meshes;
	};

}
