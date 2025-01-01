
#include "util/pffpch.h"

#include "mesh_serializer.h"

namespace PFF {

	void serialize_mesh(const std::filesystem::path filename, ref<geometry::mesh_asset> mesh_asset,
		asset_file_header& asset_header, general_mesh_file_header& general_header, static_mesh_file_header& static_mesh_header, const serializer::option option) {

		auto serializer = serializer::binary(filename, "PFF_asset_file", option);
		serialize_mesh_headers(serializer, asset_header, general_header),
			serialize_static_mesh_header(serializer, static_mesh_header);
			serializer.entry(mesh_asset->surfaces)
			.entry(mesh_asset->vertices)
			.entry(mesh_asset->indices)
			.entry(mesh_asset->bounds_data);

	}

	void serialize_mesh_headers(serializer::binary& serializer, asset_file_header& asset_header, general_mesh_file_header& general_mesh_header) {

		if (serializer.get_option() == serializer::option::save_to_file) {					// verify the file content

			CORE_ASSERT(asset_header.type == file_type::mesh, "", "Tryed to but provided asset_path is a mesh");
		}

		serializer.entry(asset_header)
		.entry(general_mesh_header);

		if (serializer.get_option() == serializer::option::load_from_file) {				// verify the file content

			CORE_ASSERT(asset_header.type == file_type::mesh, "", "Tryed to but provided asset_path is a mesh");
		}
	}

	void serialize_static_mesh_header(serializer::binary& serializer, static_mesh_file_header& static_mesh_header) {

		serializer.entry(static_mesh_header.version)
		.entry(static_mesh_header.source_file)
		.entry(static_mesh_header.mesh_index);
	}

}