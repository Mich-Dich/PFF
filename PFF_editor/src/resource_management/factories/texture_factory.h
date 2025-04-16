#pragma once

#include "engine/geometry/mesh.h"

namespace PFF::texture_factory {

    struct load_options {

        bool generate_mipmaps = true;
        bool compress_texture = false;
        bool flip_vertically = false;

        bool operator==(const load_options& other) const {
            return (generate_mipmaps   == other.generate_mipmaps &&
                    compress_texture   == other.compress_texture &&
                    flip_vertically    == other.flip_vertically);
        }
        bool operator!=(const load_options& other) const { return !(*this == other); }
    };

    // redundent because its also in the texture header

    // struct texture_file_metadata {
    //     std::string             name;
    //     std::filesystem::path   source_file;
    //     glm::vec2               size;               // size in pixel
    // };

    enum class import_fail_reason {

        invalid_format = 0,
        missing_file,
        unsupported_format,
    };

    bool check_if_assets_already_exists(const std::filesystem::path source_path, const std::filesystem::path destination_path, const load_options options, std::vector<std::string>& assets_that_already_exist);

    // factory loads external assets and convert them into engine-specific-assets in the project/content directory
    bool import_texture(const std::filesystem::path source_path, const std::filesystem::path destination_path, const load_options options);

}
