#pragma once

#include "util/pffpch.h"
#include "engine/render/renderer.h"

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

    // wierd layout because alignement limitations on GPU
    struct vertex {

        glm::vec3           position;
        float               uv_x;
        glm::vec3           normal;
        float               uv_y;
        glm::vec4           color;
    };


	class mesh {
	public:

		mesh(std::vector<u32> indices = {}, std::vector<vertex> vertices = {})
			: m_indices(indices), m_vertices(vertices) {};

		std::string							name{};
		std::vector<Geo_surface>			surfaces{};
        PFF::render::GPU_mesh_buffers	    mesh_buffers{};

		std::vector<u32>					m_indices{};
		std::vector<vertex>					m_vertices{};
	private:

	};

}