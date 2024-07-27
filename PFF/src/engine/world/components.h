#pragma once

#include "util/pffpch.h"
#include "engine/render/render_public_data.h"
#include "engine/render/material.h"

namespace PFF {

	struct transform_component {

		PFF_DEFAULT_CONSTRUCTORS(transform_component);
		transform_component(const glm::mat4& transform)
			: transform(transform) {}

		PFF_IMPLISITE_CASTS(glm::mat4, transform);

		glm::mat4 transform;
	};

	struct ID_component {

		PFF_DEFAULT_CONSTRUCTORS(ID_component);

		UUID ID;
	};

	struct tag_component {
		
		PFF_DEFAULT_CONSTRUCTORS(tag_component);
		tag_component(const std::string& tag)
			: tag(tag) {}
		
		std::string tag;
	};

	// ------------------------------------------------------------------------------------------------------------------------
	// MESH FOR RENDERER		=> need this component and the transform component
	// ------------------------------------------------------------------------------------------------------------------------

//	struct mesh{
//
//		u32					index_count;
//		u32					first_index;
//		glm::mat4			transform_offset;		// offset from the parent => can build chains
//#if defined PFF_RENDER_API_VULKAN
//		VkBuffer			index_buffer;
//		VkDeviceAddress		vertex_bffer_address;
//#endif
//		material_instance*	material;
//	};


	struct mesh_component {

		PFF_DEFAULT_CONSTRUCTORS(mesh_component);

		u32					index_count;
		u32					first_index;
		glm::mat4			transform_offset;		// offset from the parent => can build chains
#if defined PFF_RENDER_API_VULKAN
		VkBuffer			index_buffer;
		VkDeviceAddress		vertex_bffer_address;
#endif
		material_instance* material;
	};



}
