#pragma once

#include "engine/render/material.h"

namespace PFF {

	class material_asset_manager {
	public:

		//static static_mesh_asset_manager& get() { return s_instance; }

		// @breif path needs to be relative to project_dir/content
		static ref<material_inst> get_from_path(const std::filesystem::path path);

	private:

		material_asset_manager();
		static material_asset_manager s_instance;

		std::unordered_map<std::filesystem::path, ref<material>>				m_uploaded_image_assets;
	};


}