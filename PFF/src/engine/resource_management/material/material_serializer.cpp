
#include "util/pffpch.h"

#include "util/io/serializer_yaml.h"

#include "material_serializer.h"

// TODO: move to material namespace
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

		LOG(Warn, "Not implemented yet");

		if (serializer.get_option() == serializer::option::load_from_file) {				// verify the file content

			ASSERT(asset_header.type == file_type::material, "", "Tryed to deserialize mesh header, but provided asset_path is [" << asset_header_file_type_to_str(asset_header.type) << "] instead of mesh");
			// ASSERT(asset_header.type == file_type::mesh, "", "Tryed to deserialize mesh header, but provided asset_path is [  ] instead of mesh");
		}
	}
	

#define USE_YAML_FOR_TESTING
#ifdef USE_YAML_FOR_TESTING

	void serialize_material_instance(const std::filesystem::path filename, asset_file_header& asset_header, general_material_instance_file_header& general_header, specific_material_instance_file_header& specific_header, material_instance_creation_data& resources, const serializer::option option) {

		LOG(Trace, "filename [" << filename.generic_string() << "]");
		// std::filesystem::file
		serializer::yaml(filename, "PFF_asset_file", option)
			.sub_section("asset_header", [&](serializer::yaml& section) {

				section.entry(KEY_VALUE(asset_header.file_version))
					.entry(KEY_VALUE(asset_header.type))
					.entry(KEY_VALUE(asset_header.timestamp));
			}).sub_section("general_header", [&](serializer::yaml& section) {

				section.entry(KEY_VALUE(general_header.file_version));
			}).sub_section("specific_header", [&](serializer::yaml& section) {

				section.entry(KEY_VALUE(specific_header.source_file))
					.entry(KEY_VALUE(specific_header.parent_material_path));
			}).sub_section("resources", [&](serializer::yaml& section) {

				section.entry(KEY_VALUE(resources.color_texture))
					.entry(KEY_VALUE(resources.color_texture_sampler))
					.entry(KEY_VALUE(resources.metal_rough_texture))
					.entry(KEY_VALUE(resources.metal_rough_texture_sampler));
			});

	}

	void serialize_material_instance_headers(serializer::yaml& serializer, asset_file_header& asset_header, general_material_instance_file_header& general_header, specific_material_instance_file_header& specific_header, const serializer::option option) {
		if (serializer.get_option() == serializer::option::save_to_file) {					// verify the file content

			ASSERT(asset_header.type == file_type::material_instance, "", "Tryed to serialize mesh header, but provided asset type is not a mesh");
		}

		serializer.sub_section("asset_header", [&](serializer::yaml& section) {

			section.entry(KEY_VALUE(asset_header.file_version))
					.entry(KEY_VALUE(asset_header.type))
					.entry(KEY_VALUE(asset_header.timestamp));
		}).sub_section("general_header", [&](serializer::yaml& section) {

			section.entry(KEY_VALUE(general_header.file_version));
		}).sub_section("specific_header", [&](serializer::yaml& section) {

			section.entry(KEY_VALUE(specific_header.source_file))
				.entry(KEY_VALUE(specific_header.parent_material_path));
		});

		if (serializer.get_option() == serializer::option::load_from_file) {				// verify the file content

			ASSERT(asset_header.type == file_type::material_instance, "", "Tryed to deserialize mesh header, but provided asset_path is [" << asset_header_file_type_to_str(asset_header.type) << "] instead of mesh");
			// ASSERT(asset_header.type == file_type::mesh, "", "Tryed to deserialize mesh header, but provided asset_path is [  ] instead of mesh");
		}
	}

#else

	void serialize_material_instance(const std::filesystem::path filename, asset_file_header& asset_header, general_material_instance_file_header& general_header, specific_material_instance_file_header& specific_header, material_instance_creation_data resources, const serializer::option option) {

		LOG(Trace, "filename [" << filename.generic_string() << "]");
		// std::filesystem::file
		serializer::binary(filename, "PFF_asset_file", option)
			.entry(asset_header)
			.entry(general_header)
			.entry(specific_header)
			.entry(resources.color_texture)
			.entry(resources.color_texture_sampler)
			.entry(resources.metal_rough_texture)
			.entry(resources.metal_rough_texture_sampler);
	}

	void serialize_material_instance_headers(serializer::yaml& serializer, asset_file_header& asset_header, general_material_instance_file_header& general_header, specific_material_instance_file_header& specific_header, const serializer::option option) {
		// if (serializer.get_option() == serializer::option::save_to_file) {					// verify the file content

		// 	ASSERT(asset_header.type == file_type::material_instance, "", "Tryed to serialize mesh header, but provided asset type is not a mesh");
		// }

		// serializer.sub_section("asset_header", [&](serializer::yaml& section) {

		// 	section.entry(KEY_VALUE(asset_header.file_version))
		// 			.entry(KEY_VALUE(asset_header.type))
		// 			.entry(KEY_VALUE(asset_header.timestamp));
		// }).sub_section("general_header", [&](serializer::yaml& section) {

		// 	section.entry(KEY_VALUE(general_header.file_version));
		// }).sub_section("specific_header", [&](serializer::yaml& section) {

		// 	section.entry(KEY_VALUE(specific_header.source_file))
		// 		.entry(KEY_VALUE(specific_header.parent_material_path));
		// });

		// if (serializer.get_option() == serializer::option::load_from_file) {				// verify the file content

		// 	ASSERT(asset_header.type == file_type::material_instance, "", "Tryed to deserialize mesh header, but provided asset_path is [" << asset_header_file_type_to_str(asset_header.type) << "] instead of mesh");
		// 	// ASSERT(asset_header.type == file_type::mesh, "", "Tryed to deserialize mesh header, but provided asset_path is [  ] instead of mesh");
		// }
	}

#endif

}
