#pragma once

#include "util/io/serializer_yaml.h"
#include "util/io/serializer_binary.h"
#include "engine/render/material.h"
#include "material_headers.h"

#include "engine/resource_management/headers.h"
// #include "engine/resource_management/mesh_headers.h"


namespace PFF {

	
	void serialize_material(const std::filesystem::path filename, ref<material> mesh_asset, asset_file_header& asset_header, general_material_file_header& general_header, specific_material_file_header& specific_header, const serializer::option option);

	void serialize_material_headers(serializer::binary& serializer, asset_file_header& asset_header, general_material_file_header& general_header, specific_material_file_header& specific_header, const serializer::option option);
	


	void serialize_material_instance(const std::filesystem::path filename, asset_file_header& asset_header, general_material_instance_file_header& general_header, specific_material_instance_file_header& specific_header, material_instance_creation_data resources, const serializer::option option);

	void serialize_material_instance_headers(serializer::yaml& serializer, asset_file_header& asset_header, general_material_instance_file_header& general_header, specific_material_instance_file_header& specific_header, const serializer::option option);
}
