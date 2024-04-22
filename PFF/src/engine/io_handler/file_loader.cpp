
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

        std::optional<std::vector<PFF::geometry::mesh_asset>> load_gltf_meshes(std::filesystem::path filePath) {

            CORE_LOG(Trace, "Loading GLTF: " << filePath );

            fastgltf::GltfDataBuffer data;
            data.loadFromFile(filePath);

            constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
            fastgltf::Parser parser{};
            auto load = parser.loadGltfBinary(&data, filePath.parent_path(), gltfOptions);

            CORE_VALIDATE(load, return {}, "", "Failed to load glTF: [" << fastgltf::to_underlying(load.error()) << "]");

            fastgltf::Asset gltf;
            gltf = std::move(load.get());

            std::vector<geometry::mesh_asset> meshes;

            // use the same vectors for all meshes so that the memory doesnt reallocate as often
            std::vector<u32> indices;
            std::vector<geometry::vertex> vertices;
            for (fastgltf::Mesh& mesh : gltf.meshes) {

                geometry::mesh_asset new_mesh;
                new_mesh.name = mesh.name;

                // clear the mesh arrays each mesh, we dont want to merge them by error
                indices.clear();
                vertices.clear();

                for (auto&& p : mesh.primitives) {

                    geometry::Geo_surface new_surface;
                    new_surface.startIndex = (u32)indices.size();
                    new_surface.count = (u32)gltf.accessors[p.indicesAccessor.value()].count;
                    size_t initial_vtx = vertices.size();

                    // load indexes
                    {
                        fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
                        indices.reserve(indices.size() + indexaccessor.count);

                        fastgltf::iterateAccessor<u32>(gltf, indexaccessor, [&](u32 idx) {
                                indices.push_back(idx + static_cast<u32>(initial_vtx));
                            });
                    }

                    // load vertex positions
                    {
                        fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
                        vertices.resize(vertices.size() + posAccessor.count);

                        fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor, [&](glm::vec3 v, size_t index) {

                            geometry::vertex new_vertex;
                            new_vertex.position = v;
                            new_vertex.normal = { 1, 0, 0 };
                            new_vertex.color = glm::vec4{ 1.f };
                            new_vertex.uv_x = 0;
                            new_vertex.uv_y = 0;
                            vertices[initial_vtx + index] = new_vertex;
                            });
                    }

                    // load vertex normals
                    auto normals = p.findAttribute("NORMAL");
                    if (normals != p.attributes.end()) {

                        fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).second], [&](glm::vec3 v, size_t index) {
                                vertices[initial_vtx + index].normal = v;
                            });
                    }

                    // load UVs
                    auto uv = p.findAttribute("TEXCOORD_0");
                    if (uv != p.attributes.end()) {

                        fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).second], [&](glm::vec2 v, size_t index) {
                                vertices[initial_vtx + index].uv_x = v.x;
                                vertices[initial_vtx + index].uv_y = v.y;
                            });
                    }

                    // load vertex colors
                    auto colors = p.findAttribute("COLOR_0");
                    if (colors != p.attributes.end()) {

                        fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).second], [&](glm::vec4 v, size_t index) {
                                vertices[initial_vtx + index].color = v;
                            });
                    }
                    new_mesh.surfaces.push_back(new_surface);
                }

                // display the vertex normals
                constexpr bool OverrideColors = true;
                if (OverrideColors) {
                    for (geometry::vertex& vtx : vertices)
                        vtx.color = glm::vec4(vtx.normal, 1.f);
                }




                // new_mesh.meshBuffers = engine->uploadMesh(indices, vertices);





                meshes.emplace_back(geometry::mesh_asset(std::move(new_mesh)));
            }

            return meshes;

        }

    }

}
