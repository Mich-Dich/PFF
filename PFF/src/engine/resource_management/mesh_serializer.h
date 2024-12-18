#pragma once

#include "util/io/serializer_yaml.h"
#include "util/io/serializer_binary.h"
#include "engine/geometry/mesh.h"

#include "headers.h"
#include "mesh_headers.h"


namespace PFF {

	void serialize_mesh(const std::filesystem::path filename, ref<geometry::mesh_asset> mesh_asset,
		asset_file_header& asset_header, general_mesh_file_header& general_header, static_mesh_file_header& static_mesh_header, const serializer::option option);

	void serialize_mesh_headers(serializer::binary& serializer, asset_file_header& asset_header, general_mesh_file_header& general_mesh_header);
	
	void serialize_static_mesh_header(serializer::binary& serializer, static_mesh_file_header& static_mesh_header);

}
