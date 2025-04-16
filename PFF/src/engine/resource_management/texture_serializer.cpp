
#include "util/pffpch.h"

#include "texture_serializer.h"

namespace PFF {

	void serialize_texture(const std::filesystem::path filename, u32* pixel_data, asset_file_header& asset_header, general_texture_file_header& general_header, specific_texture_file_header& specific_texture_header, const serializer::option option) {

		auto serializer = serializer::binary(filename, "PFF_asset_file", option);
		serialize_texture_headers(serializer, asset_header, general_header);
		serialize_specific_texture_header(serializer, specific_texture_header);
		serializer.array<u32>(pixel_data, specific_texture_header.width * specific_texture_header.height);

	}

	void serialize_texture_headers(serializer::binary& serializer, asset_file_header& asset_header, general_texture_file_header& general_texture_header) {

		if (serializer.get_option() == serializer::option::save_to_file) {					// verify the file content

			ASSERT(asset_header.type == file_type::texture, "", "Tryed to serialize texture header, but provided asset type is not a Texture");
		}

		serializer.entry(general_texture_header.type)
			.entry(general_texture_header.file_version);
			
		if (serializer.get_option() == serializer::option::load_from_file) {				// verify the file content

			ASSERT(asset_header.type == file_type::texture, "", "Tryed to deserialize texture header, but provided asset_path is [" << asset_header_file_type_to_str(asset_header.type) << "] instead of texture");
			// ASSERT(asset_header.type == file_type::texture, "", "Tryed to deserialize texture header, but provided asset_path is [  ] instead of texture");
		}
	}
	
	void serialize_specific_texture_header(serializer::binary& serializer, specific_texture_file_header& specific_texture_header) {

		serializer.entry(specific_texture_header.name)
			.entry(specific_texture_header.source_file)
			.entry(specific_texture_header.width)
			.entry(specific_texture_header.height)
			.entry(specific_texture_header.mip_levels);
	}


}
