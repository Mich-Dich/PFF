#pragma once

#include "util/pffpch.h"
#include "engine/render/renderer.h"

namespace PFF::geometry {

    struct Geo_surface {

        u32 startIndex;
        u32 count;
    };

    struct mesh_asset {

        std::string name;
        std::vector<Geo_surface> surfaces;
        PFF::render::GPU_mesh_buffers meshBuffers;
    };

    // wierd layout because alignement limitations on GPU
    struct vertex {

        glm::vec3           position;
        float               uv_x;
        glm::vec3           normal;
        float               uv_y;
        glm::vec4           color;
    };

}