#pragma once

#include "util/pffpch.h"

#if defined PFF_RENDER_API_VULKAN
#include "engine/render/vulkan/vk_types.h"
#include "engine/render/vulkan/vk_descriptor.h"
#include "engine/render/vulkan/vk_types.h"
#endif

namespace PFF::render {

#if defined(PFF_EDITOR_DEBUG) || defined(PFF_DEBUG)
	#define PFF_RENDERER_DEBUG_CAPABILITY
#endif

	enum class render_api {

		none = 0,
		Vulkan = 1,
		OpenGl = 2,
		D3D12 = 3,
		Metal = 4,
	};

	struct GPU_mesh_buffers {

#if defined PFF_RENDER_API_VULKAN
		vulkan::vk_buffer	index_buffer{};
		vulkan::vk_buffer	vertex_buffer{};
		VkDeviceAddress		vertex_buffer_address{};
#endif
	};




	struct compute_push_constants {

		glm::vec4				data1{};
		glm::vec4				data2{};
		glm::vec4				data3{};
		glm::vec4				data4{};
	};

	struct compute_push_constants_dynamic_skybox {

		glm::mat4				inverse_view{};
		glm::vec4				middle_sky_color = glm::vec4(0.f, 0.55078f, 0.828125f, 1.f);
		glm::vec4				horizon_sky_color = glm::vec4(0.57734375f, 0.7375f, 0.7921875f, 1.f);
		glm::vec2				image_size{};
		f32 					sun_distance = 100000.f;
		f32 					sun_radius = 1000.f;																// --- allignment
    	f32						sun_core_falloff = .85f;          	// [0.8-0.99] smaller = softer core
    	f32						sun_glow_radius_multiplier = 2.f; 	// [1.5-4.0] glow halo size
    	f32						sun_glow_intensity = .5f;        	// [0.1-1.0] glow brightness
    	f32						sun_glow_falloff = 2.5f;          	// [1.0-4.0] glow edge hardness

		f32 					FOV_y = 45.f;

    	// Cloud parameters
		f32 					cloud_height = 5.f;
		f32 					time = 0.f;
		f32 					cloud_density = 0.4f;																// --- allignment
		glm::vec4 				cloud_color = glm::vec4(0.95f, 0.95f, 1.0f, 0.5f);
		glm::vec2 				cloud_speed = glm::vec2(0.1f, 0.2f);
		glm::vec2 				cloud_scale = glm::vec2(.6f, .6f);
		f32 					cloud_coverage = .5f;
		f32						cloud_persistence = .5f;
		f32						cloud_detail = .2f;
		u32						cloud_octaves = 3;																	// --- allignment
		
		// glm::vec3				wind_shear{1.f, 1.f, 1.f};      		// Forward scattering amount
		// f32						cloud_depth = .5f;      				// Cloud layer thickness					// --- allignment
		// f32						cloud_height_variation = 1.f; 			// Density to light absorption
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

		glm::mat4			view{};
		glm::mat4			proj{};
		glm::mat4			proj_view{};
		glm::vec4			ambient_color{};
		glm::vec4			sunlight_direction{};		// w for sun power
		glm::vec4			sunlight_color{};
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
