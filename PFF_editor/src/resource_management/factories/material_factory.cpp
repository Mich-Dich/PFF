
#include "util/pch_editor.h"

#include <stb_image.h>
#include "engine/render/material.h"
#include "engine/resource_management/material/material_headers.h"
#include "engine/resource_management/material/material_headers.h"
#include "engine/resource_management/material/material_serializer.h"
#include "util/io/serializer_binary.h"

#include "material_factory.h"

namespace PFF::material_factory {


    bool check_if_assets_already_exists(const std::filesystem::path& destination_path, std::vector<std::string>& assets_that_already_exist) {

        return std::filesystem::exists(destination_path);
    }

    
    bool create_instance(const std::filesystem::path& destination_path, material_instance_creation_data resources) {

        // VALIDATE(std::filesystem::is_regular_file(source_path), return false, "", "provided source path invalid [" << source_path << "]");
        // std::filesystem::path new_asset_path = destination_path / source_path.filename().replace_extension(PFF_ASSET_EXTENTION);
        // LOG(Trace, "Trying to import texture. source: " << source_path << " destination: " << new_asset_path);

        // asset_file_header loc_asset_file_header;
        // loc_asset_file_header.file_version = version{1, 0, 0};
        // loc_asset_file_header.type = file_type::material_instance;
        // loc_asset_file_header.timestamp = util::get_system_time();

        // general_material_instance_file_header loc_general_header;
        // loc_general_header.file_version = version{1, 0, 0};

        // specific_material_instance_file_header loc_specific_header;
        // loc_specific_header.source_file = source_path.generic_string();
        // loc_specific_header.parent_material_path = std::filesystem::path("/FAKE_PATH_FOR_DEV/DEV_ONLY.pffasset");           // hard code fake here and in material_asset_manger

        // auto serializer = serializer::yaml(new_asset_path, "PFF_asset_file", serializer::option::save_to_file);
        // serialize_material_instance(new_asset_path, loc_asset_file_header, loc_general_texture_file_header, loc_specific_texture_file_header, resources, serializer::option::save_to_file);
        return true;
    }

}
