#pragma once

#include "engine/geometry/mesh.h"

namespace PFF {

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
