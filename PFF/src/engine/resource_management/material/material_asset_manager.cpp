
#include "util/pffpch.h"

#include "application.h"
#include "engine/resource_management/headers.h"

#include "material_asset_manager.h"

namespace PFF {

	material_asset_manager material_asset_manager::s_instance;

	material_asset_manager::material_asset_manager() {}

	ref<material_instance> material_asset_manager::get_from_path(const std::filesystem::path path) {

		VALIDATE(path.extension() == PFF_ASSET_EXTENTION, return nullptr, "", "Provided path is not a PFF-asset");

// 		std::filesystem::path loc_path = application::get().get_project_path() / CONTENT_DIR / path;

// 		if (s_instance.m_uploaded_material_assets.find(loc_path) != s_instance.m_uploaded_material_assets.end())
// 			return s_instance.m_uploaded_material_assets[loc_path];

// 		else {

// #define SIMPLE_PROCESS__IGNORE_CUSTOM_FILE_FORMAT
// #ifdef SIMPLE_PROCESS__IGNORE_CUSTOM_FILE_FORMAT
// 			s_instance.m_uploaded_material_assets[loc_path] = create_ref<image>(path, image_format::RGBA);
// #else
// 			asset_file_header asset_file_header{};
// 			texture_file_header texture_header{};

// 			s_instance.m_uploaded_material_assets[loc_path] = create_ref<image>();
// 			serialize_texture(loc_path, s_instance.m_uploaded_material_assets[loc_path], asset_file_header, texture_header, serializer::option::load_from_file);
// 			ASSERT(asset_file_header.type == file_type::texture, "", "Tryed to load an asset file that is not a mesh");
// #endif

// 			return s_instance.m_uploaded_material_assets[loc_path];
// 		}

		return nullptr;
	}

}
