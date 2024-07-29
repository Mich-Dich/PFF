#pragma once

#include "util/pffpch.h"
#include "engine/render/render_public_data.h"

namespace PFF::geometry {

    struct Geo_surface {

        u32 startIndex;
        u32 count;
    };

    //struct mesh_asset {
    //
    //    std::string name;
    //    std::vector<Geo_surface> surfaces;
    //    PFF::render::vk_GPU_mesh_buffers mesh_buffers;
    //};

    // wierd layout because of alignement limitations on GPU
    struct vertex {

        vertex() {}
        vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec4& col, float u_x, float u_y)
            : position(pos), normal(norm), color(col), uv_x(u_x), uv_y(u_y) {}

        glm::vec3                           position{};
        float                               uv_x = 0;
        glm::vec3                           normal{};
        float                               uv_y = 0;
        glm::vec4                           color{};
    };


    struct mesh_asset {

        mesh_asset(std::vector<u32> indices = {}, std::vector<vertex> vertices = {})
			: m_indices(indices), m_vertices(vertices) {};

		std::string							name{};
		std::vector<Geo_surface>			surfaces{};
        PFF::render::GPU_mesh_buffers	    mesh_buffers{};
		std::vector<u32>					m_indices{};
		std::vector<vertex>					m_vertices{};
	};
//
//    struct render_object {
//
//        u32					index_count;
//        u32					first_index;
//        glm::mat4			transform_offset;		// offset from the parent => can build chains
//        material_instance*  material;
//#if defined PFF_RENDER_API_VULKAN
//        VkBuffer			index_buffer;
//        VkDeviceAddress		vertex_buffer_address;
//#endif
//    };


}
