
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


    std::optional<std::vector<std::string>> load_gltf_meshes_names(std::filesystem::path file_path) {

        VALIDATE(std::filesystem::exists(file_path), return {}, "", "provided file path does not exist");

        fastgltf::GltfDataBuffer data;
        data.loadFromFile(file_path);

        constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
        fastgltf::Parser parser{};
        auto load = parser.loadGltfBinary(&data, file_path.parent_path(), gltfOptions);
        VALIDATE(load, return {}, "", "Failed to load glTF: [" << fastgltf::getErrorMessage(load.error()) << "]");

        fastgltf::Asset gltf;
        gltf = std::move(load.get());

        std::vector<std::string> mesh_names{};
        for (fastgltf::Mesh& mesh : gltf.meshes)
            mesh_names.push_back(std::string(mesh.name.c_str()));           // TODO: rewrite this

        return mesh_names;
    }


    bool check_if_assets_already_exists(const std::filesystem::path source_path, const std::filesystem::path destination_path, const load_options options, std::vector<std::string>& assets_that_already_exist) {

        VALIDATE(std::filesystem::exists(source_path) && (source_path.extension() == ".gltf" || source_path.extension() == ".glb")&& std::filesystem::is_regular_file(source_path), return false, "", "provided source path invalid [" << source_path << "]");
        LOG(Trace, "Checking if gltf mesh can be imported. source: " << source_path << " destination: " << destination_path);

        assets_that_already_exist.clear();
        bool any_assest_already_exists = false;

        if (options.combine_meshes) {

            const std::filesystem::path output_path = destination_path / (source_path.filename().replace_extension(PFF_ASSET_EXTENTION));
            return std::filesystem::exists(output_path);
        }

        std::optional<std::vector<std::string>> loc_mesh_assets = load_gltf_meshes_names(source_path);
        if (!loc_mesh_assets.has_value())
            return false;

        for (const auto mesh : loc_mesh_assets.value()) {

            std::string name_buffer = (options.include_file_name_in_asset_name) ? source_path.filename().replace_extension("").string() + "_" + mesh : mesh;
            std::filesystem::path output_path = destination_path / (name_buffer + PFF_ASSET_EXTENTION);
            if (std::filesystem::exists(output_path)) {

                LOG(Trace, "Mesh with name of [" << name_buffer << "] already exists");
                any_assest_already_exists = true;
                assets_that_already_exist.push_back(name_buffer);
            }
        }

        return any_assest_already_exists;
    }

    
    bool import_gltf_mesh(const std::filesystem::path source_path, const std::filesystem::path destination_path, const load_options options) {

        VALIDATE(std::filesystem::exists(source_path) && (source_path.extension() == ".gltf" || source_path.extension() == ".glb")&& std::filesystem::is_regular_file(source_path), return false, "", "provided source path invalid [" << source_path << "]");
        LOG(Trace, "Trying to import gltf mesh. source: " << source_path << " destination: " << destination_path);

        // load file from source_path and
        std::optional<std::unordered_map<std::string, ref<PFF::geometry::mesh_asset>>> loc_mesh_assets = load_gltf_meshes(source_path);
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
