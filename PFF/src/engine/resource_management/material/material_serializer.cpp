
#include "util/pffpch.h"

#include "material_serializer.h"

namespace PFF {

	void serialize_material(const std::filesystem::path filename, ref<material> mesh_asset, asset_file_header& asset_header, general_material_file_header& general_header, specific_material_file_header& specific_header, const serializer::option option) {

		auto serializer = serializer::binary(filename, "PFF_asset_file", option);
		serialize_material_headers(serializer, asset_header, general_header, specific_header, option);
	// 	serializer.entry(mesh_asset->surfaces)
	// 		.entry(mesh_asset->vertices)
	// 		.entry(mesh_asset->indices)
	// 		.entry(mesh_asset->bounds_data);
	}

	void serialize_material_headers(serializer::binary& serializer, asset_file_header& asset_header, general_material_file_header& general_header, specific_material_file_header& specific_header, const serializer::option option) {
		if (serializer.get_option() == serializer::option::save_to_file) {					// verify the file content

			ASSERT(asset_header.type == file_type::material, "", "Tryed to serialize mesh header, but provided asset type is not a mesh");
		}

		serializer.entry(asset_header)
			.entry(general_header)
			.entry(specific_header);

		if (serializer.get_option() == serializer::option::load_from_file) {				// verify the file content

			ASSERT(asset_header.type == file_type::material, "", "Tryed to deserialize mesh header, but provided asset_path is [" << asset_header_file_type_to_str(asset_header.type) << "] instead of mesh");
			// ASSERT(asset_header.type == file_type::mesh, "", "Tryed to deserialize mesh header, but provided asset_path is [  ] instead of mesh");
		}
	}
	


	void serialize_material_instance(const std::filesystem::path filename, ref<material_instance> mesh_asset, asset_file_header& asset_header, general_material_instance_file_header& general_header, specific_material_instance_file_header& specific_header, const serializer::option option) {

		auto serializer = serializer::binary(filename, "PFF_asset_file", option);
		serialize_material_instance_headers(serializer, asset_header, general_header, specific_header, option);
	// 	serializer.entry(mesh_asset->surfaces)
	// 		.entry(mesh_asset->vertices)
	// 		.entry(mesh_asset->indices)
	// 		.entry(mesh_asset->bounds_data);
	}

	void serialize_material_instance_headers(serializer::binary& serializer, asset_file_header& asset_header, general_material_instance_file_header& general_header, specific_material_instance_file_header& specific_header, const serializer::option option) {
		if (serializer.get_option() == serializer::option::save_to_file) {					// verify the file content

			ASSERT(asset_header.type == file_type::material_instance, "", "Tryed to serialize mesh header, but provided asset type is not a mesh");
		}

		serializer.entry(asset_header)
			.entry(general_header)
			.entry(specific_header);

		if (serializer.get_option() == serializer::option::load_from_file) {				// verify the file content

			ASSERT(asset_header.type == file_type::material_instance, "", "Tryed to deserialize mesh header, but provided asset_path is [" << asset_header_file_type_to_str(asset_header.type) << "] instead of mesh");
			// ASSERT(asset_header.type == file_type::mesh, "", "Tryed to deserialize mesh header, but provided asset_path is [  ] instead of mesh");
		}
	}

}
