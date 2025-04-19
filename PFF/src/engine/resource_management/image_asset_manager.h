#pragma once

#include "engine/render/image.h"

namespace PFF {

	class image_asset_manager {
	public:

		//static static_mesh_asset_manager& get() { return s_instance; }

		// @breif path needs to be relative to project_dir/content
		static ref<image> get_from_path(const std::filesystem::path path);

		void release_all();
		
	private:

		image_asset_manager();
		static image_asset_manager s_instance;

		std::unordered_map<std::filesystem::path, ref<image>>				m_uploaded_image_assets;
	};


}