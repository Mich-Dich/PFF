#pragma once

#include "engine/render/material.h"

namespace PFF {

	class material_asset_manager {
	public:

		//static static_mesh_asset_manager& get() { return s_instance; }

		// @breif path needs to be relative to project_dir/content
		static ref<material> get_material_from_path(const std::filesystem::path path);
		static ref<material_instance> get_material_instance_from_path(const std::filesystem::path path);

		static void shutdown();

	private:

		material_asset_manager();
		static material_asset_manager s_instance;

		std::unordered_map<std::filesystem::path, ref<material>>				m_uploaded_material_assets;
		std::unordered_map<std::filesystem::path, ref<material_instance>>		m_uploaded_material_inst_assets;
	};

}
