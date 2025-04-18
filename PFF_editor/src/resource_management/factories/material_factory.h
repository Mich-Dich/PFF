#pragma once

#include "engine/render/material.h"

namespace PFF::material_factory {


    // struct texture_metadata {
    //     int width;
    //     int height;
    //     int channels;
    //     std::string format;   // e.g., "RGB", "RGBA", or "Grayscale"
    //     int mip_levels;
    // };

    // struct load_options {

    //     bool generate_mipmaps = true;
    //     bool compress_texture = false;
    //     bool flip_x = false;
    //     bool flip_y = false;

    //     bool operator==(const load_options& other) const {
    //         return (generate_mipmaps    == other.generate_mipmaps &&
    //                 compress_texture    == other.compress_texture &&
    //                 flip_x              == other.flip_x &&
    //                 flip_y              == other.flip_y);
    //     }
    //     bool operator!=(const load_options& other) const { return !(*this == other); }
    // };

    // redundent because its also in the texture header

    // struct texture_file_metadata {
    //     std::string             name;
    //     std::filesystem::path   source_file;
    //     glm::vec2               size;               // size in pixel
    // };

    // enum class import_fail_reason {

    //     invalid_format = 0,
    //     missing_file,
    //     unsupported_format,
    // };
    
    // //
    // bool get_metadata(const std::filesystem::path& source_path, texture_metadata &metadata);

    //
    bool check_if_assets_already_exists(const std::filesystem::path& destination_path, std::vector<std::string>& assets_that_already_exist);


    bool create_instance(const std::filesystem::path& destination_path, material_instance_creation_data resources);

}
