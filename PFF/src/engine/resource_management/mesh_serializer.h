#pragma once

#include "headers.h"
#include "mesh_headers.h"
#include "engine/geometry/mesh.h"

namespace PFF {

	PFF_API void serialize_mesh(const std::filesystem::path filename, ref<geometry::mesh_asset> mesh_asset,
		asset_file_header& asset_header, general_mesh_file_header& general_header, static_mesh_file_header& static_mesh_header, const serializer::option option);

	PFF_API void serialize_mesh_headers(serializer::binary& serializer, asset_file_header& asset_header, general_mesh_file_header& general_mesh_header);
	
	PFF_API void serialize_static_mesh_header(serializer::binary& serializer, static_mesh_file_header& static_mesh_header);

}
