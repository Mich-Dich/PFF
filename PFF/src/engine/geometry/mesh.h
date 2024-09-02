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
    
    struct PFF_API bounds {

        glm::vec3   origin{};
        f32         sphere_radius{};
        glm::vec3   extents{};          // AABB
    };
    
    struct PFF_API Geo_surface {

        u32         startIndex;
        u32         count;
        bounds      bounds;
        //ref<material_instatnce> material;
    };


    // wierd layout because of alignement limitations on GPU
    struct PFF_API vertex {

        vertex() {}
        vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec4& col, const f32 uv_x, const f32 uv_y)
            : position(pos), normal(norm), color(col), uv_x(uv_x), uv_y(uv_y) {}
        vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec4& col, const glm::vec2 uv)
            : position(pos), normal(norm), color(col), uv_x(uv.x), uv_y(uv.y) {}

        glm::vec3                           position{};
        f32                                 uv_x = 0;
        glm::vec3                           normal{};
        f32                                 uv_y = 0;
        glm::vec4                           color{};
    };

    struct PFF_API mesh_asset {

        mesh_asset(std::vector<u32> indices = {}, std::vector<vertex> vertices = {})
			: indices(indices), vertices(vertices) {};

        void calc_bounds();

        //std::string                         name{};
        PFF::render::GPU_mesh_buffers	    mesh_buffers{};     // GPU side buffers
		std::vector<Geo_surface>			surfaces{};
		std::vector<u32>					indices{};
		std::vector<vertex>					vertices{};
        bounds                              bounds{};
	};



}
