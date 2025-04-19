
#include "util/pffpch.h"

#if defined PFF_RENDER_API_VULKAN
	// #include <vulkan/vulkan.h>
	#include "engine/render/vulkan/vk_image.h"
	#include "engine/render/vulkan/vk_initializers.h"
	#include "engine/render/vulkan/vk_types.h"
#endif

#include "application.h"
#include "engine/resource_management/headers.h"
#include "engine/resource_management/image_asset_manager.h"
#include "util/io/serializer_yaml.h"
#include "util/io/serializer_binary.h"
#include "engine/render/material.h"
#include "material_serializer.h"
#include "material_headers.h"

#include "material_asset_manager.h"

namespace PFF {

	material_asset_manager material_asset_manager::s_instance;

	material_asset_manager::material_asset_manager() {}

	
	ref<material> material_asset_manager::get_material_from_path(const std::filesystem::path path) {
		
		VALIDATE(path.extension() == PFF_ASSET_EXTENTION, return nullptr, "", "Provided path is not a PFF-asset");
		return nullptr;
	}

	ref<material_instance> material_asset_manager::get_material_instance_from_path(const std::filesystem::path path) {
		
		VALIDATE(path.extension() == PFF_ASSET_EXTENTION, return nullptr, "", "Provided path is not a PFF-asset");

		// 1	check for parent material	(disabled for dev, only one master material)

		// 2 	get MI resources from the file
		asset_file_header asset_header{};
		general_material_instance_file_header general_header{};
		specific_material_instance_file_header specific_header{};
		material_instance_creation_data resources{};
		serialize_material_instance(path, asset_header, general_header, specific_header, resources, serializer::option::load_from_file);
		// VALIDATE( , return nullptr, "", "Failed to deserialize material instance [" << path.generic_string() << "]");
		
		LOG(Trace, "asset_header.file_version [" << asset_header.file_version << "]");
		LOG(Trace, "asset_header.type [" << (u32)asset_header.type << "]");
		// LOG(Trace, "asset_header.timestamp [" << asset_header.timestamp << "]");

		LOG(Trace, "general_header.file_version [" << general_header.file_version << "]");

		LOG(Trace, "general_header.source_file [" << specific_header.source_file.generic_string() << "]");
		LOG(Trace, "general_header.parent_material_path [" << specific_header.parent_material_path.generic_string() << "]");

		LOG(Trace, "color image path [" << resources.color_texture.generic_string() << "]");
		LOG(Trace, "metal_rough image path [" << resources.metal_rough_texture.generic_string() << "]");

		// 3	create an instance from the parent and save ref		
		material::material_resources material_resources;						// TODO: this should be material_type specific		(eg. all opace materials have the same struct)
		material_resources.color_image = image_asset_manager::get_from_path(resources.color_texture);
		material_resources.color_sampler = GET_RENDERER.get_default_sampler_nearest();
		material_resources.metal_rough_image = image_asset_manager::get_from_path(resources.metal_rough_texture);
		material_resources.metal_rough_sampler = GET_RENDERER.get_default_sampler_nearest();
		material_resources.data_buffer = GET_RENDERER.get_material_constant_buffer();
		material_resources.data_buffer_offset = 0;
		return create_ref<material_instance>(GET_RENDERER.get_metal_rough_material_ref().create_instance(material_pass::main_color, material_resources));
		
	}


}

/*
struct material_instance_creation_data {

	std::filesystem::path				color_texture{};
	DEV_ONLY_sampler					color_texture_sampler{};
	std::filesystem::path				metal_rough_texture{};
	DEV_ONLY_sampler					metal_rough_texture_sampler{};
};
*/