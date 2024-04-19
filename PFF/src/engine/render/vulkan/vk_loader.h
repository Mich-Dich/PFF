#pragma once

#include "vk_types.h"


/// <summary>
/// 
/// 
/// 
/// 
///  TODO: MOVE THIS INTO CUSTOM SUBSYSTEM 
/// 
/// 
/// 
/// 
/// </summary>

namespace PFF::render::vulkan {

    class vk_renderer;

    namespace loader{

        struct Geo_surface {
            u32 startIndex;
            u32 count;
        };

        struct MeshAsset {

            std::string name;
            std::vector<Geo_surface> surfaces;
            GPU_mesh_buffers meshBuffers;
        };

        std::optional<std::vector<std::shared_ptr<MeshAsset>>> load_gltf_meshes(std::filesystem::path filePath);

    }
}
