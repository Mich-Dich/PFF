#pragma once

#include "util/pffpch.h"

#if defined PFF_RENDER_API_VULKAN
#include "engine/render/vulkan/vk_types.h"
#include "engine/render/vulkan/vk_descriptor.h"
#include "engine/render/vulkan/vk_types.h"
#endif

namespace PFF::render {

	enum class material_pass : u8 {

		MainColor,
		Transparent,
		Other
	};

	struct material_pipeline {

#if defined PFF_RENDER_API_VULKAN
		VkPipeline			pipeline;
		VkPipelineLayout	layout;
#endif
	};

	struct material_instance {

		material_pipeline* pipeline;
#if defined PFF_RENDER_API_VULKAN
		VkDescriptorSet		material_set;
#endif
		material_pass		pass_type;
	};

}