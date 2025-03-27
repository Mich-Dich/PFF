#pragma once

#include "image.h"
#include "render_util.h"

#if defined PFF_RENDER_API_VULKAN
	#include "vulkan/vk_descriptor.h"
	#include "vulkan/vk_types.h"
#endif 

namespace PFF {

	enum class material_pass : u8 {

		main_color,
		transparent,
		other
	};

	struct material_pipeline {

#if defined PFF_RENDER_API_VULKAN
		VkPipeline							pipeline{};
		VkPipelineLayout					layout{};
#endif
	};

	struct material_instance {

		material_pass						pass_type{};
		material_pipeline*					pipeline{};
#if defined PFF_RENDER_API_VULKAN
		VkDescriptorSet						material_set{};
#endif
	};


	struct material {

		material_pipeline					opaque_pipeline{};
		material_pipeline					transparent_pipeline{};

		struct material_constants {
			glm::vec4						color_factors;
			glm::vec4						metal_rough_factors;
			glm::vec4						extra[14];		// padding to get to 256 bytes, we need it anyway for uniform buffers
		};

		struct material_resources {
			ref<image>						color_image;
			VkSampler						color_sampler;
			ref<image>						metal_rough_image;
			VkSampler						metal_rough_sampler;
			VkBuffer						data_buffer;
			u32								data_buffer_offset;
		};

#if defined PFF_RENDER_API_VULKAN
		render::vulkan::descriptor_writer	writer;
		VkDescriptorSetLayout				material_layout{};
		
		void build_pipelines();
		void release_resources();

		material_instance create_instance(material_pass pass, const material_resources& resources, render::vulkan::descriptor_allocator_growable& descriptor_allocator);
#endif

	};

#ifdef PFF_RENDERER_DEBUG_CAPABILITY
	
	void build_debug_lines_pipelines(material& material);

#endif // PFF_RENDERER_DEBUG_CAPABILITY

}
