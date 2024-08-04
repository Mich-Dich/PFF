
#include "util/pffpch.h"

#include "stb_image.h"
#include <iostream>

#include "engine/render/renderer.h"
//#include "vk_renderer.h"
//#include "vk_initializers.h"
//#include "vk_types.h"
#include <glm/gtx/quaternion.hpp>

#include <fastgltf/base64.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/util.hpp>
#include <fastgltf/tools.hpp>

#include "file_loader.h"


namespace PFF::IO {

    namespace mesh_loader {

        std::optional<std::vector<ref<PFF::geometry::mesh_asset>>> load_gltf_meshes(std::filesystem::path file_path) {

            CORE_VALIDATE(std::filesystem::exists(file_path), return {}, "Loading GLTF: " << file_path, "provided file path does not exist");

            fastgltf::GltfDataBuffer data;
            data.loadFromFile(file_path);

            constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
            fastgltf::Parser parser{};
            auto load = parser.loadGltfBinary(&data, file_path.parent_path(), gltfOptions);

            CORE_VALIDATE(load, return {}, "", "Failed to load glTF: [" << fastgltf::to_underlying(load.error()) << "]");

            fastgltf::Asset gltf;
            gltf = std::move(load.get());

            std::vector<ref<geometry::mesh_asset>> meshes{};

            // use the same vectors for all meshes so that the memory doesnt reallocate as often

            //std::vector<u32> indices;
            //std::vector<geometry::vertex> vertices;
            for (fastgltf::Mesh& mesh : gltf.meshes) {

                geometry::mesh_asset loc_mesh{};
                loc_mesh.name = mesh.name;

                CORE_LOG(Trace, "name of surface: " << mesh.name)

                // clear the mesh arrays each mesh, we dont want to merge them by error
                loc_mesh.m_indices.clear();
                loc_mesh.m_vertices.clear();

                for (auto&& p : mesh.primitives) {

                    CORE_LOG(Trace, "name of surface: " << mesh.name)

                    geometry::Geo_surface new_surface;
                    new_surface.startIndex = (u32)loc_mesh.m_indices.size();
                    new_surface.count = (u32)gltf.accessors[p.indicesAccessor.value()].count;
                    size_t initial_vtx = loc_mesh.m_vertices.size();

                    // load indexes
                    {
                        fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
                        loc_mesh.m_indices.reserve(loc_mesh.m_indices.size() + indexaccessor.count);

                        fastgltf::iterateAccessor<u32>(gltf, indexaccessor, [&](u32 idx) {
                            loc_mesh.m_indices.push_back(idx + static_cast<u32>(initial_vtx));
                            });
                    }

                    // load vertex positions
                    {
                        fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
                        loc_mesh.m_vertices.resize(loc_mesh.m_vertices.size() + posAccessor.count);

                        fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor, [&](glm::vec3 v, size_t index) {

                            geometry::vertex new_vertex;
                            new_vertex.position = v;
                            new_vertex.normal = { 1, 0, 0 };
                            new_vertex.color = glm::vec4{ 1.f };
                            new_vertex.uv_x = 0;
                            new_vertex.uv_y = 0;
                            loc_mesh.m_vertices[initial_vtx + index] = new_vertex;
                            });
                    }

                    // load vertex normals
                    auto normals = p.findAttribute("NORMAL");
                    if (normals != p.attributes.end()) {

                        fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).second], [&](glm::vec3 v, size_t index) {
                            loc_mesh.m_vertices[initial_vtx + index].normal = v;
                            });
                    }

                    // load UVs
                    auto uv = p.findAttribute("TEXCOORD_0");
                    if (uv != p.attributes.end()) {

                        fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).second], [&](glm::vec2 v, size_t index) {
                                loc_mesh.m_vertices[initial_vtx + index].uv_x = v.x;
                                loc_mesh.m_vertices[initial_vtx + index].uv_y = v.y;
                            });
                    }

                    // load vertex colors
                    auto colors = p.findAttribute("COLOR_0");
                    if (colors != p.attributes.end()) {

                        fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).second], [&](glm::vec4 v, size_t index) {
                            loc_mesh.m_vertices[initial_vtx + index].color = v;
                            });
                    }

                    //loop the vertices of this surface, find min/max bounds
                    glm::vec3 minpos = loc_mesh.m_vertices[initial_vtx].position;
                    glm::vec3 maxpos = loc_mesh.m_vertices[initial_vtx].position;
                    for (size_t x = initial_vtx; x < loc_mesh.m_vertices.size(); x++) {
                        minpos = glm::min(minpos, loc_mesh.m_vertices[x].position);
                        maxpos = glm::max(maxpos, loc_mesh.m_vertices[x].position);
                    }
                    // calculate origin and extents from the min/max, use extent lenght for radius
                    new_surface.bounds.origin = (maxpos + minpos) / 2.f;
                    new_surface.bounds.extents = (maxpos - minpos) / 2.f;
                    new_surface.bounds.sphereRadius = glm::length(new_surface.bounds.extents);

                    loc_mesh.surfaces.push_back(new_surface);
                }

                // display the vertex normals
                constexpr bool OverrideColors = true;
                if (OverrideColors) {
                    for (geometry::vertex& vtx : loc_mesh.m_vertices)
                        vtx.color = glm::vec4(vtx.normal, 1.f);
                }

                // calc mesh_asset bounds based on surfaces
                glm::vec3 minpos = loc_mesh.surfaces[0].bounds.origin - loc_mesh.surfaces[0].bounds.extents;
                glm::vec3 maxpos = loc_mesh.surfaces[0].bounds.origin + loc_mesh.surfaces[0].bounds.extents;
                for (size_t x = 0; x < loc_mesh.surfaces.size(); x++) {
                    minpos = glm::min(minpos, loc_mesh.surfaces[x].bounds.origin - loc_mesh.surfaces[x].bounds.extents);
                    maxpos = glm::max(maxpos, loc_mesh.surfaces[x].bounds.origin + loc_mesh.surfaces[x].bounds.extents);
                }
                loc_mesh.bounds.origin = (maxpos + minpos) / 2.f;
                loc_mesh.bounds.extents = (maxpos - minpos) / 2.f;
                loc_mesh.bounds.sphereRadius = glm::length(loc_mesh.bounds.extents);

                // new_mesh.meshBuffers = engine->uploadMesh(indices, vertices);
                meshes.emplace_back(create_ref<geometry::mesh_asset>(std::move(loc_mesh)));
            }

            return meshes;
        }

    }

}
