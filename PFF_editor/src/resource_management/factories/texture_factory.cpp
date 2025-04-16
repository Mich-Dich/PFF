
#include "util/pch_editor.h"

#include <stb_image.h>
// #include <iostream>
// #include <glm/gtx/quaternion.hpp>
// #include <fastgltf/base64.hpp>
// #include <fastgltf/core.hpp>
// #include <fastgltf/types.hpp>
// #include <fastgltf/glm_element_traits.hpp>
// #include <fastgltf/util.hpp>
// #include <fastgltf/tools.hpp>

#include "engine/resource_management/headers.h"
#include "engine/resource_management/texture_headers.h"
#include "engine/resource_management/texture_serializer.h"
#include "engine/render/image.h"

#include "static_mesh_factory.h"

namespace PFF::texture_factory {

    bool check_if_assets_already_exists(const std::filesystem::path source_path, const std::filesystem::path destination_path, const load_options options, std::vector<std::string>& assets_that_already_exist) {

        VALIDATE(std::filesystem::exists(source_path) && (source_path.extension() == ".png")&& std::filesystem::is_regular_file(source_path), return false, "", "provided source path invalid [" << source_path << "]");
        LOG(Trace, "Checking if texture can be imported. source: " << source_path << " destination: " << destination_path);
       
        // Form the output path by replacing the extension with your engine's asset extension.
        const std::filesystem::path output_path = destination_path / (source_path.filename().replace_extension(PFF_ASSET_EXTENTION));
        // Optionally, track the asset if it already exists.
        if (std::filesystem::exists(output_path)) {
            assets_that_already_exist.push_back(output_path.string());
            return true;
        }
        return false;
    }

    
    bool import_texture(const std::filesystem::path source_path, const std::filesystem::path destination_path, const load_options options) {

        VALIDATE(std::filesystem::exists(source_path) && (source_path.extension() == ".png") && std::filesystem::is_regular_file(source_path), return false, "", "provided source path invalid [" << source_path << "]");
        LOG(Trace, "Trying to import texture. source: " << source_path << " destination: " << destination_path);

        asset_file_header loc_asset_file_header;
        loc_asset_file_header.file_version = version{1, 0, 0};
        loc_asset_file_header.type = file_type::texture;
        loc_asset_file_header.timestamp = util::get_system_time();

        int width = 0, height = 0, channels = 0;
        stbi_uc* pixelData = stbi_load(file_path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);                                         // Force the image to load as RGBA (4 channels)
        VALIDATE(pixelData, return false, "Loading texture: " << file_path, "Failed to load image using stb_image");

        general_texture_file_header loc_general_texture_file_header;
        loc_general_texture_file_header.type = texture_type::texture2D;                             // FORCE simple 2D for now
        loc_general_texture_file_header.version = version{1, 0, 0};

        specific_texture_file_header loc_specific_texture_file_header;
        loc_specific_texture_file_header.name = source_path.filename().string();
        loc_specific_texture_file_header.version = version{1, 0, 0};
        loc_specific_texture_file_header.source_file = source_path.generic_string();
        loc_specific_texture_file_header.width = width;
        loc_specific_texture_file_header.height = height;
        loc_specific_texture_file_header.mip_levels = 0;                                            // FORCE for now

        // Since we forced 4 channels, we can reinterpret the pixel data as an array of u32 values (each pixel consists of 4 bytes: R, G, B, A)         Note: This assumes your platform's endianness is compatible.
        u32* pixels = reinterpret_cast<u32*>(pixelData);
        
        bool result = serialize_texture(destination_path, pixels, loc_asset_file_header, loc_general_texture_file_header, loc_specific_texture_file_header);
        stbi_image_free(pixelData);
        return result;
    }

}
