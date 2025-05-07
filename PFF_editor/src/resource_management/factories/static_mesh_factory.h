#pragma once

#include "engine/geometry/mesh.h"

namespace PFF::mesh_factory {

    struct load_options {

        bool        combine_meshes = false;
        bool        include_file_name_in_asset_name = true;
        bool        auto_generate_LODs = true;
        bool        generate_smooth_normals = true;
        bool        triangulate = true;
        bool        flipUVs = false;
        bool        join_identical_vertices = true;

        bool operator==(const load_options& other) {

            return (combine_meshes == other.combine_meshes 
                && include_file_name_in_asset_name  == other.include_file_name_in_asset_name 
                && auto_generate_LODs               == other.auto_generate_LODs
                && generate_smooth_normals          == other.generate_smooth_normals
                && triangulate                      == other.triangulate
                && flipUVs                          == other.flipUVs
                && join_identical_vertices          == other.join_identical_vertices);
        }
        bool operator!=(const load_options& other) { return !(*this == other); }
    };

    struct metadata {
        std::string     file_format;        // Detected file format (e.g., "FBX", "glTF")
        size_t          file_size;          // index of this mesh from the original gltf-file
        u32             num_meshes;         // Total sub-meshes in file
        u32             total_vertices;     // Combined vertex count across all meshes
        u32             total_faces;        // Combined face count across all meshes
        bool            has_normals;        // Any mesh contains normals
        bool            has_uvs;            // Any mesh contains texture coordinates
        bool            has_vertex_colors;  // Any mesh contains vertex colors
        bool            has_bones;          // Any mesh contains skeletal data
        u32             num_materials;      // Total materials in file
        u32             num_animations;     // Total animations in file

    };

    enum class import_fail_reason {

    };


    bool get_metadata(const std::filesystem::path& source_path, metadata &metadata);


    bool check_if_assets_already_exist(const std::filesystem::path& source_path, const std::filesystem::path& destination_path, const load_options options, std::vector<std::string>& assets_that_already_exist);

    // factory loads external assets and convert them into engine-specific-assets in the project/content directory
    bool import_mesh(const std::filesystem::path source_path, const std::filesystem::path destination_path, const load_options options, f32& progression);

}
