#pragma once

#include "util/pffpch.h"
#include "engine/render/render_public_data.h"
#include "engine/render/material.h"

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

        u32         startIndex = 0;
        u32         count = 0;
        bounds      bounds{};

        // MAYBE:   bool        should_render = false;

        //ref<material_instatnce> material;
    };


    // wierd layout because of alignement limitations on GPU
    struct vertex {

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

    struct mesh_asset {

        mesh_asset(std::vector<u32> indices = {}, std::vector<vertex> vertices = {})
			: indices(indices), vertices(vertices) {};

        void calc_bounds();

        //std::string                         name{};
        PFF::render::GPU_mesh_buffers	    mesh_buffers{};     // GPU side buffers
		std::vector<Geo_surface>			surfaces{};         // used for LODs
		std::vector<u32>					indices{};
		std::vector<vertex>					vertices{};
        bounds                              bounds{};
	};

    struct procedural_mesh_asset : public mesh_asset {

        procedural_mesh_asset(std::vector<u32> indices = {}, std::vector<vertex> vertices = {})
            : mesh_asset(indices, vertices) {}
        //~procedural_mesh_asset();

        render::vulkan::vk_buffer           staging_buffer = {};
        void*               staging_data = nullptr;
        size_t              staging_buffer_size = 0;
        
        material_instance*  material = nullptr;		// TODO: currently uses one material for all surfaces in mesh_asset, change so it can use diffrent materials per furface

        void update(const std::vector<u32>& new_indices, const std::vector<vertex>& new_vertices);
        void cleanup();
    };

}
