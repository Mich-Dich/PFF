#pragma once

#include "util/pffpch.h"

namespace PFF {

	static version		current_asset_file_header_version(1, 0, 0);

	enum class file_type : u32 {

		none,
		world,
		map,
		mesh,
		audio,
		texture,
		material,
		material_instance,

	};

	struct asset_file_header {

		version				file_version{};
		file_type			type{};
		system_time			timestamp{};
	};

}
