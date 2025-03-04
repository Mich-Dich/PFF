
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

#include "engine/resource_management/headers.h"
#include "engine/resource_management/mesh_headers.h"
#include "engine/resource_management/mesh_serializer.h"

#include "static_mesh_factory.h"

namespace PFF::mesh_factory {

    // PRIVATE
    ref<PFF::geometry::mesh_asset> load_gltf_mesh(std::filesystem::path filePath);


    std::optional<std::unordered_map<std::string, ref<PFF::geometry::mesh_asset>>> load_gltf_meshes(std::filesystem::path file_path) {

        VALIDATE(std::filesystem::exists(file_path), return {}, "Loading GLTF: " << file_path, "provided file path does not exist");

        fastgltf::GltfDataBuffer data;
        data.loadFromFile(file_path);

        constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
        fastgltf::Parser parser{};
        auto load = parser.loadGltfBinary(&data, file_path.parent_path(), gltfOptions);

        VALIDATE(load, return {}, "", "Failed to load glTF: [" << fastgltf::getErrorMessage(load.error()) << "]");

        fastgltf::Asset gltf;
        gltf = std::move(load.get());

        std::unordered_map<std::string, ref<geometry::mesh_asset>> meshes{};

        // use the same vectors for all meshes so that the memory doesnt reallocate as often

        //std::vector<u32> indices;
        //std::vector<geometry::vertex> vertices;
        for (fastgltf::Mesh& mesh : gltf.meshes) {

            geometry::mesh_asset loc_mesh{};
            //loc_mesh.name = mesh.name;

            LOG(Trace, "name of surface: " << mesh.name)

            // clear the mesh arrays each mesh, we dont want to merge them by error
            loc_mesh.indices.clear();
            loc_mesh.vertices.clear();

            for (auto&& p : mesh.primitives) {

                LOG(Trace, "name of surface: " << mesh.name)

                geometry::Geo_surface new_surface;
                new_surface.startIndex = (u32)loc_mesh.indices.size();
                new_surface.count = (u32)gltf.accessors[p.indicesAccessor.value()].count;
                size_t initial_vtx = loc_mesh.vertices.size();

                // load indexes
                {
                    fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
                    loc_mesh.indices.reserve(loc_mesh.indices.size() + indexaccessor.count);

                    fastgltf::iterateAccessor<u32>(gltf, indexaccessor, [&](u32 idx) {
                        loc_mesh.indices.push_back(idx + static_cast<u32>(initial_vtx));
                        });
                }

                // load vertex positions
                {
                    fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
                    loc_mesh.vertices.resize(loc_mesh.vertices.size() + posAccessor.count);

                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor, [&](glm::vec3 v, size_t index) {

                        geometry::vertex new_vertex;
                        new_vertex.position = v;
                        new_vertex.normal = { 1, 0, 0 };
                        new_vertex.color = glm::vec4{ 1.f };
                        new_vertex.uv_x = 0;
                        new_vertex.uv_y = 0;
                        loc_mesh.vertices[initial_vtx + index] = new_vertex;
                        });
                }

                // load vertex normals
                auto normals = p.findAttribute("NORMAL");
                if (normals != p.attributes.end()) {

                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).second], [&](glm::vec3 v, size_t index) {
                        loc_mesh.vertices[initial_vtx + index].normal = v;
                        });
                }

                // load UVs
                auto uv = p.findAttribute("TEXCOORD_0");
                if (uv != p.attributes.end()) {

                    fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).second], [&](glm::vec2 v, size_t index) {
                        loc_mesh.vertices[initial_vtx + index].uv_x = v.x;
                        loc_mesh.vertices[initial_vtx + index].uv_y = v.y;
                        });
                }

                // load vertex colors
                auto colors = p.findAttribute("COLOR_0");
                if (colors != p.attributes.end()) {

                    fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).second], [&](glm::vec4 v, size_t index) {
                        loc_mesh.vertices[initial_vtx + index].color = v;
                        });
                }

                //loop the vertices of this surface, find min/max bounds
                glm::vec3 minpos = loc_mesh.vertices[initial_vtx].position;
                glm::vec3 maxpos = loc_mesh.vertices[initial_vtx].position;
                for (size_t x = initial_vtx; x < loc_mesh.vertices.size(); x++) {
                    minpos = glm::min(minpos, loc_mesh.vertices[x].position);
                    maxpos = glm::max(maxpos, loc_mesh.vertices[x].position);
                }
                // calculate origin and extents from the min/max, use extent lenght for radius
                new_surface.bounds_data.origin = (maxpos + minpos) / 2.f;
                new_surface.bounds_data.extents = (maxpos - minpos) / 2.f;
                new_surface.bounds_data.sphere_radius = glm::length(new_surface.bounds_data.extents);

                loc_mesh.surfaces.push_back(new_surface);
            }

            // display the vertex normals
            constexpr bool OverrideColors = true;
            if (OverrideColors) {
                for (geometry::vertex& vtx : loc_mesh.vertices)
                    vtx.color = glm::vec4(vtx.normal, 1.f);
            }

            // calc mesh_asset bounds based on surfaces
            glm::vec3 minpos = loc_mesh.surfaces[0].bounds_data.origin - loc_mesh.surfaces[0].bounds_data.extents;
            glm::vec3 maxpos = loc_mesh.surfaces[0].bounds_data.origin + loc_mesh.surfaces[0].bounds_data.extents;
            for (size_t x = 0; x < loc_mesh.surfaces.size(); x++) {
                minpos = glm::min(minpos, loc_mesh.surfaces[x].bounds_data.origin - loc_mesh.surfaces[x].bounds_data.extents);
                maxpos = glm::max(maxpos, loc_mesh.surfaces[x].bounds_data.origin + loc_mesh.surfaces[x].bounds_data.extents);
            }
            loc_mesh.bounds_data.origin = (maxpos + minpos) / 2.f;
            loc_mesh.bounds_data.extents = (maxpos - minpos) / 2.f;
            loc_mesh.bounds_data.sphere_radius = glm::length(loc_mesh.bounds_data.extents);

            // new_mesh.meshBuffers = engine->uploadMesh(indices, vertices);
            meshes.emplace(mesh.name, create_ref<geometry::mesh_asset>(std::move(loc_mesh)));
        }

        return meshes;
    }


    bool import_gltf_mesh(const std::filesystem::path source_path, const std::filesystem::path destination_path, const load_options options) {

        // TODO: check if source_path is valid
        // TODO: check if destination_path is free

        LOG(Trace, "source_path: " << source_path << " destination_path: " << destination_path);

        // load file from source_path and
        std::optional<std::unordered_map<std::string, ref<PFF::geometry::mesh_asset>>> loc_mesh_assets = load_gltf_meshes(source_path);
        VALIDATE(loc_mesh_assets.has_value(), return false, "loaded source file", "Failed to load meshes");

        if (options.combine_meshes) {

            //static_mesh_file_metadata metadata{};
            //metadata.mesh_index = 0;

            //LOG(Warn, "Not implemented yet");                  // <= HERE
            //return false;



            ref<PFF::geometry::mesh_asset> combined_mesh = std::make_shared<PFF::geometry::mesh_asset>();                           // Create a new mesh to hold the combined data

            // Variables to keep track of the current vertex and index count
            size_t current_vertex_count = 0;
            size_t current_index_count = 0;

            for (const auto& mesh_pair : loc_mesh_assets.value()) {                                                                 // Iterate through each mesh and combine them
                const auto& mesh = mesh_pair.second;

                combined_mesh->vertices.insert(combined_mesh->vertices.end(), mesh->vertices.begin(), mesh->vertices.end());        // Append vertices to the combined mesh

                for (const auto& index : mesh->indices)                                                                            // Adjust indices and append to the combined mesh
                    combined_mesh->indices.push_back(index + static_cast<u32>(current_vertex_count));

                current_vertex_count += mesh->vertices.size();                                                                      // Update the current vertex count
            }

            // Create metadata for the combined mesh
            static_mesh_file_metadata metadata{};
            metadata.name = source_path.filename().replace_extension("").string() + "_combined";

            asset_file_header asset_header{};
            asset_header.type = file_type::mesh;
            asset_header.file_version = current_asset_file_header_version;
            asset_header.timestamp = util::get_system_time();

            general_mesh_file_header general_mesh_header{};
            general_mesh_header.type = mesh_type::static_mesh;

            static_mesh_file_header static_mesh_header{};
            static_mesh_header.version = 1;
            static_mesh_header.source_file = source_path;
            static_mesh_header.mesh_index = 0;

            // Serialize the combined mesh to the destination path
            std::filesystem::path output_path = destination_path / (metadata.name + PFF_ASSET_EXTENTION);
            serialize_mesh(output_path, combined_mesh, asset_header, general_mesh_header, static_mesh_header, serializer::option::save_to_file);

            return true;

        } else {

            u32 counter = 0;
            for (const auto mesh : loc_mesh_assets.value()) {

                static_mesh_file_metadata metadata{};
                metadata.name = source_path.filename().replace_extension("").string() + "_" + mesh.first;

                asset_file_header asset_header{};
                asset_header.type = file_type::mesh;
                asset_header.file_version = current_asset_file_header_version;
                asset_header.timestamp = util::get_system_time();

                general_mesh_file_header general_mesh_header{};
                general_mesh_header.type = mesh_type::static_mesh;

                static_mesh_file_header static_mesh_header{};
                static_mesh_header.version = 1;
                static_mesh_header.source_file = source_path;
                static_mesh_header.mesh_index = counter;
                counter++;

                std::filesystem::path output_path = destination_path / (metadata.name + PFF_ASSET_EXTENTION);
                serialize_mesh(output_path, mesh.second, asset_header, general_mesh_header, static_mesh_header, serializer::option::save_to_file);
            }
        }

        LOG(Trace, "FUNCTION END");

        return true;
    }

}
