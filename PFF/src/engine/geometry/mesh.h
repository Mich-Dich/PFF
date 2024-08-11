#pragma once

#include "util/pffpch.h"
#include "engine/render/render_public_data.h"

namespace PFF {

    enum class mobility {
        locked,
        movable,
        dynamic,
    };

}

namespace PFF::geometry {
    
    struct bounds {     

        glm::vec3   origin{};
        f32         sphere_radius{};
        glm::vec3   extents{};          // AABB
    };
    
    struct Geo_surface {

        u32         startIndex;
        u32         count;
        bounds      bounds;
        //ref<material_instatnce> material;
    };


    // wierd layout because of alignement limitations on GPU
    struct vertex {

        vertex() {}
        vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec4& col, f32 u_x, f32 u_y)
            : position(pos), normal(norm), color(col), uv_x(u_x), uv_y(u_y) {}

        glm::vec3                           position{};
        f32                                 uv_x = 0;
        glm::vec3                           normal{};
        f32                                 uv_y = 0;
        glm::vec4                           color{};
    };

    struct mesh_asset {

        mesh_asset(std::vector<u32> indices = {}, std::vector<vertex> vertices = {})
			: indices(indices), vertices(vertices) {};

        std::string                         name;
        PFF::render::GPU_mesh_buffers	    mesh_buffers{};     // GPU side buffers
		std::vector<Geo_surface>			surfaces{};
		std::vector<u32>					indices{};
		std::vector<vertex>					vertices{};
        bounds                              bounds;

	};

}
