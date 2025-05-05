
#include "util/pch_editor.h"

#include <stb_image.h>
#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include <fastgltf/base64.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/util.hpp>
#include <fastgltf/tools.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "engine/resource_management/headers.h"
#include "engine/resource_management/mesh_headers.h"
#include "engine/resource_management/mesh_serializer.h"

#include "static_mesh_factory.h"

namespace PFF::mesh_factory {

    // PRIVATE
    ref<PFF::geometry::mesh_asset> load_gltf_mesh(std::filesystem::path filePath);

    std::optional<std::unordered_map<std::string, ref<PFF::geometry::mesh_asset>>> load_assimp_meshes(const std::filesystem::path& file_path) {
            
        VALIDATE(std::filesystem::exists(file_path), return {}, "Loading Assimp: " << file_path, "provided file path does not exist");

        Assimp::Importer importer;

        // Post‐processing: triangulate, generate normals, flip UVs, join identical vertices
        const aiScene* scene = importer.ReadFile(
            file_path.string(),
            aiProcess_Triangulate
            | aiProcess_GenSmoothNormals
            | aiProcess_FlipUVs
            | aiProcess_JoinIdenticalVertices
        );

        VALIDATE(scene && scene->HasMeshes(), return {}, "", "Assimp failed to load model: " << importer.GetErrorString());
        std::unordered_map<std::string, ref<PFF::geometry::mesh_asset>> meshes;

        for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
            const aiMesh* aimesh = scene->mMeshes[m];
            PFF::geometry::mesh_asset loc_mesh{};
            loc_mesh.surfaces.clear();
            loc_mesh.indices.clear();
            loc_mesh.vertices.clear();
            loc_mesh.vertices.reserve(aimesh->mNumVertices);                        // Reserve space
            loc_mesh.indices.reserve(aimesh->mNumFaces * 3);                        // Each face is a triangle (3 indices)

            // Extract vertices
            for (unsigned int i = 0; i < aimesh->mNumVertices; ++i) {
                glm::vec3 pos{ aimesh->mVertices[i].x,
                            aimesh->mVertices[i].y,
                            aimesh->mVertices[i].z };
                glm::vec3 norm{ 0.f, 0.f, 0.f };
                if (aimesh->HasNormals()) {
                    norm = { aimesh->mNormals[i].x,
                            aimesh->mNormals[i].y,
                            aimesh->mNormals[i].z };
                }
                glm::vec4 col{ 1.f };
                if (aimesh->HasVertexColors(0)) {
                    auto& c = aimesh->mColors[0][i];
                    col = { c.r, c.g, c.b, c.a };
                }
                float u = 0, v = 0;
                if (aimesh->HasTextureCoords(0)) {
                    u = aimesh->mTextureCoords[0][i].x;
                    v = aimesh->mTextureCoords[0][i].y;
                }
                loc_mesh.vertices.emplace_back(pos, norm, col, u, v);
            }

            // Extract indices & build surfaces
            size_t idxBase = 0;
            for (unsigned int f = 0; f < aimesh->mNumFaces; ++f) {
                const aiFace& face = aimesh->mFaces[f];
                PFF::geometry::Geo_surface surf{};
                surf.startIndex = static_cast<u32>(loc_mesh.indices.size());
                surf.count = static_cast<u32>(face.mNumIndices);

                // Append indices
                for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                    loc_mesh.indices.push_back(
                        static_cast<u32>(face.mIndices[k] + idxBase)
                    );
                }
                // Compute bounds for this surface
                glm::vec3 minp = loc_mesh.vertices[idxBase].position;
                glm::vec3 maxp = minp;
                for (size_t vi = idxBase;
                    vi < loc_mesh.vertices.size();
                    ++vi) {
                    minp = glm::min(minp, loc_mesh.vertices[vi].position);
                    maxp = glm::max(maxp, loc_mesh.vertices[vi].position);
                }
                surf.bounds_data.origin = (minp + maxp) * 0.5f;
                surf.bounds_data.extents = (maxp - minp) * 0.5f;
                surf.bounds_data.sphere_radius =
                    glm::length(surf.bounds_data.extents);

                loc_mesh.surfaces.push_back(surf);
                idxBase += 0; // vertices were all pushed up front
            }

            // Compute overall mesh bounds
            {
                auto& first = loc_mesh.surfaces.front().bounds_data;
                glm::vec3 gmin = first.origin - first.extents;
                glm::vec3 gmax = first.origin + first.extents;
                for (auto& s : loc_mesh.surfaces) {
                    gmin = glm::min(gmin, s.bounds_data.origin - s.bounds_data.extents);
                    gmax = glm::max(gmax, s.bounds_data.origin + s.bounds_data.extents);
                }
                loc_mesh.bounds_data.origin = (gmin + gmax) * 0.5f;
                loc_mesh.bounds_data.extents = (gmax - gmin) * 0.5f;
                loc_mesh.bounds_data.sphere_radius =
                    glm::length(loc_mesh.bounds_data.extents);
            }

            // Store by name (fallback to index if no name)
            std::string name = aimesh->mName.C_Str();
            if (name.empty()) name = "mesh_" + std::to_string(m);

            meshes.emplace(
                name,
                create_ref<PFF::geometry::mesh_asset>(std::move(loc_mesh))
            );
        }

        return meshes;
    }


    inline bool validate_mesh_path(const std::filesystem::path& path) {

        VALIDATE(std::filesystem::exists(path) && std::filesystem::is_regular_file(path), return false, "", "provided source path invalid [" << path << "]");
        const bool supported_file_type = (path.extension() == ".fbx" ||
            path.extension() == ".gltf" ||
            path.extension() == ".glb" 	||
            path.extension() == ".obj" 	||
            path.extension() == ".stl" 	||
            path.extension() == ".3mf" 	||
            path.extension() == ".dae" 	||
            path.extension() == ".xml" 	||
            path.extension() == ".ply" 	||
            path.extension() == ".plyb" ||
            path.extension() == ".3ds");
        VALIDATE(supported_file_type, return false, "", "Tried to import invalid file type [" << path << "]");
        return true;
    }

    std::optional<std::vector<std::string>> load_assimp_meshes_names(const std::filesystem::path& file_path) {

        // Validate file exists and is a glTF/glb       (only for now)
        VALIDATE(validate_mesh_path(file_path), return {}, "", "provided file path does not exist or is not suported");
    
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(file_path.string(), aiProcess_ValidateDataStructure);                  // We don't need full postprocessing for just names, but at least load meshes
        VALIDATE(scene && scene->HasMeshes(), return {}, "", "Assimp failed to load model: " << importer.GetErrorString());
    
        std::vector<std::string> mesh_names;
        mesh_names.reserve(scene->mNumMeshes);
        for (unsigned int mi = 0; mi < scene->mNumMeshes; ++mi) {
            const aiMesh* m = scene->mMeshes[mi];
            std::string name = m->mName.C_Str();
            if (name.empty()) {
                // fallback to indexed name
                name = "mesh_" + std::to_string(mi);
            }
            mesh_names.push_back(name);
        }
    
        return mesh_names;
    }

    
    bool check_if_assets_already_exist(const std::filesystem::path& source_path, const std::filesystem::path& destination_path, const load_options options, std::vector<std::string>& assets_that_already_exist) {
        
        VALIDATE(validate_mesh_path(source_path), return {}, "", "provided file path does not exist or is not suported");
        LOG(Trace, "Checking if model meshes can be imported. source: " << source_path << " destination: " << destination_path);
        assets_that_already_exist.clear();
    
        // If combining into one asset, just check for single output file
        if (options.combine_meshes) {
            std::filesystem::path out = destination_path / source_path.filename().replace_extension(PFF_ASSET_EXTENTION);
            return std::filesystem::exists(out);
        }
    
        // Otherwise, get each mesh name and check individually
        auto maybe_names = load_assimp_meshes_names(source_path);
        if (!maybe_names.has_value()) {
            // failure in loading mesh names
            return false;
        }
    
        bool any_exists = false;
        for (auto const& mesh_name : *maybe_names) {
            // prepend filename if requested
            std::string final_name = mesh_name;
            if (options.include_file_name_in_asset_name) {
                std::string base = source_path.filename().stem().string();
                final_name = base + "_" + mesh_name;
            }
            std::filesystem::path out = destination_path / (final_name + PFF_ASSET_EXTENTION);
    
            if (std::filesystem::exists(out)) {
                LOG(Trace, "Mesh asset [" << final_name << "] already exists");
                any_exists = true;
                assets_that_already_exist.push_back(final_name);
            }
        }
    
        return any_exists;
    }

    
    bool import_gltf_mesh(const std::filesystem::path source_path, const std::filesystem::path destination_path, const load_options options) {

        VALIDATE(validate_mesh_path(source_path), return {}, "", "provided file path does not exist or is not suported");
        LOG(Trace, "Trying to import gltf mesh. source: " << source_path << " destination: " << destination_path);

        // load file from source_path and
        std::optional<std::unordered_map<std::string, ref<PFF::geometry::mesh_asset>>> loc_mesh_assets = load_assimp_meshes(source_path);
        VALIDATE(loc_mesh_assets.has_value(), return false, "loaded source file", "Failed to load meshes");

        if (options.combine_meshes) {

            ref<geometry::mesh_asset> combined_mesh = create_ref<geometry::mesh_asset>();
            // combined_mesh->name = "combined_mesh";
        
            for (const auto& [mesh_name, mesh] : loc_mesh_assets.value()) {
                size_t vertex_offset = combined_mesh->vertices.size();
                size_t index_offset = combined_mesh->indices.size();
        
                // Append vertices
                combined_mesh->vertices.insert(
                    combined_mesh->vertices.end(),
                    mesh->vertices.begin(),
                    mesh->vertices.end()
                );
        
                // Append indices with vertex offset
                for (u32 index : mesh->indices) {
                    combined_mesh->indices.push_back(index + static_cast<u32>(vertex_offset));
                }
        
                // Adjust surface indices and add to combined mesh
                for (const auto& surface : mesh->surfaces) {
                    geometry::Geo_surface adjusted_surface = surface;
                    adjusted_surface.startIndex += static_cast<u32>(index_offset);
                    combined_mesh->surfaces.push_back(adjusted_surface);
                }
            }
        
            // Calculate combined bounds
            if (!combined_mesh->surfaces.empty()) {
                glm::vec3 minpos = combined_mesh->surfaces[0].bounds_data.origin - combined_mesh->surfaces[0].bounds_data.extents;
                glm::vec3 maxpos = combined_mesh->surfaces[0].bounds_data.origin + combined_mesh->surfaces[0].bounds_data.extents;
        
                for (const auto& surface : combined_mesh->surfaces) {
                    glm::vec3 surface_min = surface.bounds_data.origin - surface.bounds_data.extents;
                    glm::vec3 surface_max = surface.bounds_data.origin + surface.bounds_data.extents;
                    minpos = glm::min(minpos, surface_min);
                    maxpos = glm::max(maxpos, surface_max);
                }
        
                combined_mesh->bounds_data.origin = (maxpos + minpos) / 2.f;
                combined_mesh->bounds_data.extents = (maxpos - minpos) / 2.f;
                combined_mesh->bounds_data.sphere_radius = glm::length(combined_mesh->bounds_data.extents);
            }
        
            // Serialize combined mesh
            asset_file_header asset_header{};
            asset_header.type = file_type::mesh;
            asset_header.file_version = current_asset_file_header_version;
            asset_header.timestamp = util::get_system_time();
        
            general_mesh_file_header general_mesh_header{};
            general_mesh_header.type = mesh_type::static_mesh;
        
            static_mesh_file_header static_mesh_header{};
            static_mesh_header.name = source_path.filename().replace_extension("").string();
            static_mesh_header.version = 1;
            static_mesh_header.source_file = source_path;
            static_mesh_header.mesh_index = 0;
            static_mesh_header.mesh_bounds = combined_mesh->bounds_data;
        
            std::filesystem::path output_path = destination_path / (static_mesh_header.name + PFF_ASSET_EXTENTION);
            serialize_mesh(output_path, combined_mesh, asset_header, general_mesh_header, static_mesh_header, serializer::option::save_to_file);
        
        } else {

            u32 counter = 0;
            for (const auto mesh : loc_mesh_assets.value()) {

                asset_file_header asset_header{};
                asset_header.type = file_type::mesh;
                asset_header.file_version = current_asset_file_header_version;
                asset_header.timestamp = util::get_system_time();

                general_mesh_file_header general_mesh_header{};
                general_mesh_header.type = mesh_type::static_mesh;

                static_mesh_file_header static_mesh_header{};
                static_mesh_header.name = (options.include_file_name_in_asset_name) ? source_path.filename().replace_extension("").string() + "_" + mesh.first : mesh.first;
                static_mesh_header.version = 1;
                static_mesh_header.source_file = source_path;
                static_mesh_header.mesh_index = counter;
                static_mesh_header.mesh_bounds = mesh.second->bounds_data;
                counter++;

                std::filesystem::path output_path = destination_path / (static_mesh_header.name + PFF_ASSET_EXTENTION);
                serialize_mesh(output_path, mesh.second, asset_header, general_mesh_header, static_mesh_header, serializer::option::save_to_file);
            }
        }

        return true;
    }

}
