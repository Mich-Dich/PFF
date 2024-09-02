#pragma once

#include "util/pffpch.h"

#if defined PFF_RENDER_API_VULKAN
#include "engine/render/vulkan/vk_types.h"
#include "engine/render/vulkan/vk_descriptor.h"
#include "engine/render/vulkan/vk_types.h"
#endif

namespace PFF::render {

	enum class render_api {

		none = 0,
		Vulkan = 1,
		OpenGl = 2,
		D3D12 = 3,
		Metal = 4,
	};

	struct PFF_API GPU_mesh_buffers {

#if defined PFF_RENDER_API_VULKAN
		vk_buffer			index_buffer{};
		vk_buffer			vertex_buffer{};
		VkDeviceAddress		vertex_buffer_address{};
#endif
	};



	struct compute_push_constants {

		glm::vec4				data1;
		glm::vec4				data2;
		glm::vec4				data3;
		glm::vec4				data4;
	};

	struct compute_effect {

		const char*					name{};
		compute_push_constants		data{};
#if defined PFF_RENDER_API_VULKAN
		VkPipeline					pipeline{};
		VkPipelineLayout			layout{};
#endif
	};

	struct GPU_scene_data {

		glm::mat4			view;
		glm::mat4			proj;
		glm::mat4			proj_view;
		glm::vec4			ambient_color;
		glm::vec4			sunlight_direction;		// w for sun power
		glm::vec4			sunlight_color;
	};
	
	
//	enum class material_pass : u8 {
//
//		MainColor,
//		Transparent,
//		Other
//	};
//
//	struct material_pipeline {
//
//#if defined PFF_RENDER_API_VULKAN
//		VkPipeline			pipeline;
//		VkPipelineLayout	layout;
//#endif
//	};
//
//	struct material_instance {
//
//		material_pipeline* pipeline;
//#if defined PFF_RENDER_API_VULKAN
//		VkDescriptorSet		material_set;
//#endif
//		material_pass		pass_type;
//	};

}