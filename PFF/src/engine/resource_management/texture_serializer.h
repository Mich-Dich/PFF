#pragma once

#include "util/io/serializer_yaml.h"
#include "util/io/serializer_binary.h"
#include "engine/render/image.h"

#include "headers.h"
#include "texture_headers.h"


namespace PFF {

	void serialize_texture(const std::filesystem::path filename, u32*& pixel_data, asset_file_header& asset_header, general_texture_file_header& general_header, specific_texture_file_header& static_mesh_header, const serializer::option option);

	void serialize_texture_headers(serializer::binary& serializer, asset_file_header& asset_header, general_texture_file_header& general_mesh_header);
	
	void serialize_specific_texture_header(serializer::binary& serializer, specific_texture_file_header& specific_texture_header);

}
