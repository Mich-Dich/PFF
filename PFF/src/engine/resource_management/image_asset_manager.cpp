
#include "util/pffpch.h"

#include "application.h"
#include "headers.h"
#include "texture_headers.h"
#include "texture_serializer.h"

#include "image_asset_manager.h"

namespace PFF {

	image_asset_manager image_asset_manager::s_instance;

	image_asset_manager::image_asset_manager() {}

	void image_asset_manager::release_all() {

		LOG(Trace, "releasing all material instances")
		for (auto& [path, image_ref] : s_instance.m_uploaded_image_assets) {

			VALIDATE(image_ref, continue, "", "entry in map has invalid image ref for [" << path.generic_string() << "]")
			LOG(Trace, "releasing image for [" << path.generic_string() << "]")
			image_ref.reset();
		}
		s_instance.m_uploaded_image_assets.clear();
	}

	ref<image> image_asset_manager::get_from_path(const std::filesystem::path path) {

		VALIDATE(path.extension() == PFF_ASSET_EXTENTION, return nullptr, "", "Provided path is not a PFF-asset, path [" << path.generic_string() << "]");

		std::filesystem::path loc_path = PROJECT_PATH / CONTENT_DIR / path;

		if (s_instance.m_uploaded_image_assets.find(loc_path) != s_instance.m_uploaded_image_assets.end())
			return s_instance.m_uploaded_image_assets[loc_path];

		else {

		if (loc_path.extension() == PFF_ASSET_EXTENTION) {

			u32* pixel_data;
			asset_file_header loc_asset_header{};
			general_texture_file_header loc_general_header{};
			specific_texture_file_header loc_specific_texture_header{};
			serialize_texture(loc_path, pixel_data, loc_asset_header, loc_general_header, loc_specific_texture_header, serializer::option::load_from_file);
	
			s_instance.m_uploaded_image_assets[loc_path] = create_ref<image>(pixel_data, loc_specific_texture_header.width, loc_specific_texture_header.height, image_format::RGBA);
	
			delete pixel_data;
			return s_instance.m_uploaded_image_assets[loc_path];

		}

#define SIMPLE_PROCESS__IGNORE_CUSTOM_FILE_FORMAT
#ifdef SIMPLE_PROCESS__IGNORE_CUSTOM_FILE_FORMAT
			s_instance.m_uploaded_image_assets[loc_path] = create_ref<image>(loc_path, image_format::RGBA);
#else
			asset_file_header asset_file_header{};
			texture_file_header texture_header{};

			s_instance.m_uploaded_image_assets[loc_path] = create_ref<image>();
			serialize_texture(loc_path, s_instance.m_uploaded_image_assets[loc_path], asset_file_header, texture_header, serializer::option::load_from_file);
			ASSERT(asset_file_header.type == file_type::texture, "", "Tryed to load an asset file that is not a mesh");
#endif

			return s_instance.m_uploaded_image_assets[loc_path];
		}

		return nullptr;
	}

}
