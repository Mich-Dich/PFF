#pragma once

#include "engine/geometry/mesh.h"

namespace PFF::mesh_factory {

    struct load_options {

        bool        combine_meshes = false;
        bool        auto_generate_LODs = true;
    };

    struct static_mesh_file_metadata {

        std::string             name;
        std::filesystem::path   source_file;
        size_t                  mesh_index;         // index of this mesh from the original gltf-file
    };

    enum class import_fail_reason {

    };


    // factories should load external assets and convert them into engine-specific-assets in the project/content directory
    bool import_gltf_mesh(const std::filesystem::path source_path, const std::filesystem::path destination_path, const load_options options);


}


// C:\CustomGameEngine\PFF\test_project\content\meshes