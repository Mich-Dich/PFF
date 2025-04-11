#pragma once

#include "util/pffpch.h"

namespace PFF {

	static version		current_asset_file_header_version(1, 0, 0);

	enum class file_type : u32 {

		none = 0,
		world,
		map,						// map is a sun section of world "chunk of world"
		audio,
		mesh,
		texture,
		material,
		material_instance,
	};

	static const char* asset_header_file_type_to_str(const file_type type) {

		switch (type) {
			case file_type::none: 					return "none";
			case file_type::world: 					return "world";
			case file_type::map: 					return "map";
			case file_type::audio: 					return "audio";
			case file_type::mesh: 					return "mesh";
			case file_type::texture: 				return "texture";
			case file_type::material: 				return "material";
			case file_type::material_instance: 		return "material_instance";
			default:								return "unknown";
		}
	}

	struct asset_file_header {

		version				file_version{};
		file_type			type{};
		system_time			timestamp{};
	};

}
