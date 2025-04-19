
#include "util/pffpch.h"

#include "application.h"
#include "engine/resource_management/headers.h"
#include "engine/resource_management/image_asset_manager.h"
#include "engine/render/material.h"
#include "material_headers.h"
#include "material_serializer.h"

#include "material_asset_manager.h"

namespace PFF {

	material_asset_manager material_asset_manager::s_instance;

	material_asset_manager::material_asset_manager() {}

	void material_asset_manager::release_all() {

		LOG(Trace, "releasing all material instances")
		for (auto& [path, mat_int_ref] : s_instance.m_uploaded_material_inst_assets) {

			VALIDATE(mat_int_ref, continue, "", "entry in map has invalid material_instance ref for [" << path.generic_string() << "]")
			
			LOG(Trace, "releasing instance for [" << path.generic_string() << "]")
			GET_RENDERER.get_metal_rough_material_ref().release_instance(*mat_int_ref);
			mat_int_ref.reset();
		}
		s_instance.m_uploaded_material_inst_assets.clear();
	}

	
	ref<material> material_asset_manager::get_material_from_path(const std::filesystem::path path) {
		
		VALIDATE(path.extension() == PFF_ASSET_EXTENTION, return nullptr, "", "Provided path is not a PFF-asset");
		return nullptr;
	}

	ref<material_instance> material_asset_manager::get_material_instance_from_path(const std::filesystem::path path) {
		
		VALIDATE(path.extension() == PFF_ASSET_EXTENTION, return nullptr, "", "Provided path is not a PFF-asset");

		if (s_instance.m_uploaded_material_inst_assets.find(path) != s_instance.m_uploaded_material_inst_assets.end())
			return s_instance.m_uploaded_material_inst_assets[path];

		else {

			std::filesystem::path absolute_path = application::get().get_project_path() / CONTENT_DIR / path;
			// 1	check for parent material	(disabled for dev, only one master material)

			// 2 	get MI resources from the file
			asset_file_header asset_header{};
			general_material_instance_file_header general_header{};
			specific_material_instance_file_header specific_header{};
			material_instance_creation_data resources{};
			serialize_material_instance(absolute_path, asset_header, general_header, specific_header, resources, serializer::option::load_from_file);
			VALIDATE(!resources.color_texture.empty() && !resources.metal_rough_texture.empty(), return nullptr, "", "Failed to deserialize material instance [" << path.generic_string() << "] color image path [" << resources.color_texture.generic_string() << "] metal_rough image path [" << resources.metal_rough_texture.generic_string() << "]");

			// 3	create an instance from the parent and save ref		
			material::material_resources material_resources;						// TODO: this should be material_type specific		(eg. all opace materials have the same struct)
			material_resources.color_image = image_asset_manager::get_from_path(resources.color_texture);
			material_resources.color_sampler = GET_RENDERER.get_default_sampler_nearest();
			material_resources.metal_rough_image = image_asset_manager::get_from_path(resources.metal_rough_texture);
			material_resources.metal_rough_sampler = GET_RENDERER.get_default_sampler_nearest();
			material_resources.data_buffer = GET_RENDERER.get_material_constant_buffer();
			material_resources.data_buffer_offset = 0;
			s_instance.m_uploaded_material_inst_assets[path] = create_ref<material_instance>(GET_RENDERER.get_metal_rough_material_ref().create_instance(material_pass::main_color, material_resources));
			return s_instance.m_uploaded_material_inst_assets[path];
		}
	}

}
