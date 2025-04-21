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

		glm::vec4				data1;
		glm::vec4				data2;
		glm::vec4				data3;
		glm::vec4				data4;
	};

	struct compute_push_constants_dynamic_skybox {

		glm::vec3				basic_sky_color;
		f32 					sun_distance;
		f32 					sun_radius;
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









// Im programming in C++ with Vulkan and need some help:
// I have this pipeline I create for my compute shader and I want to supply it with not only the push constants but also a desciptor set.
// I have a var "m_scene_data" that I update at the beginning of any frame and then bind to a descriptor, and I want to use this descriptor in my compute shader as well.

// This is how I create the pipeline for my compute shader:
// 	VkPushConstantRange skybox_push_const{};
// 	skybox_push_const.offset = 0;
// 	skybox_push_const.size = sizeof(render::compute_push_constants_dynamic_skybox);
// 	skybox_push_const.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

// 	VkPipelineLayoutCreateInfo skybox_compute_lyout_CI{};
// 	skybox_compute_lyout_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
// 	skybox_compute_lyout_CI.pNext = nullptr;
// 	skybox_compute_lyout_CI.pSetLayouts = &m_draw_image_descriptor_layout;
// 	skybox_compute_lyout_CI.setLayoutCount = 1;
// 	skybox_compute_lyout_CI.pPushConstantRanges = &skybox_push_const;
// 	skybox_compute_lyout_CI.pushConstantRangeCount = 1;
// 	VK_CHECK_S(vkCreatePipelineLayout(m_device, &skybox_compute_lyout_CI, nullptr, &m_skybox_pipeline_layout));

// 	compute_pipeline_CI.layout = m_skybox_pipeline_layout;

// 	VkShaderModule skybox_shader;
// 	ASSERT(util::load_shader_module(PFF::util::get_executable_path() / "../PFF/shaders/dynamic_skybox.comp.spv", m_device, &skybox_shader), "", "Error when building the compute shader [dynamic_skybox]");
// 	compute_pipeline_CI.stage.module = skybox_shader;		//change the shader module only

// 	//change the shader module only
// 	VK_CHECK_S(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_CI, nullptr, &m_skybox_pipeline));
// 	vkDestroyShaderModule(m_device, skybox_shader, nullptr);



// This is how I execute my compute shader:
// 	render::compute_push_constants_dynamic_skybox skybox_data{};
// 	skybox_data.basic_sky_color = glm::vec3(0.3f, 0.15f, 0.25f);
// 	skybox_data.sun_distance = 1000.f;
// 	skybox_data.sun_radius = .05f;

// 	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_skybox_pipeline);
// 	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_skybox_pipeline_layout, 0, 1, &m_draw_image_descriptors, 0, nullptr);		// bind desc_set containing the draw_image for compute pipeline
// 	vkCmdPushConstants(cmd, m_skybox_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(skybox_data), &skybox_data);
// 	vkCmdDispatch(cmd, static_cast<u32>(std::ceil(m_draw_extent.width / 16.0)), static_cast<u32>(std::ceil(m_draw_extent.height / 16.0)), 1);




// This is how I update the scene_data:
// 	m_scene_data.view = m_active_camera->get_view();
// 	m_scene_data.proj = glm::perspective(glm::radians(m_active_camera->get_perspective_fov_y()), (float)m_draw_extent.width / (float)m_draw_extent.height, 100000.f, 0.1f);
// 	m_active_camera->force_set_projection_matrix(m_scene_data.proj);
// 	m_scene_data.proj[1][1] *= -1;				// invert the Y direction on projection matrix
// 	m_scene_data.proj_view = m_scene_data.proj * m_scene_data.view;

// 	// allocate a new uniform buffer for the scene data
// 	vk_buffer gpuSceneDataBuffer = create_buffer(sizeof(render::GPU_scene_data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
// 	get_current_frame().del_queue.push_func([this, gpuSceneDataBuffer]() { destroy_buffer(gpuSceneDataBuffer); });

// 	//write the buffer
// 	render::GPU_scene_data* scene_uniform_data = (render::GPU_scene_data*)gpuSceneDataBuffer.allocation->GetMappedData();
// 	*scene_uniform_data = m_scene_data;

// 	//create a descriptor set that binds that buffer and update it
// 	m_global_descriptor = get_current_frame().frame_descriptors.allocate(m_device, m_gpu_scene_data_descriptor_layout);
// 	descriptor_writer writer;
// 	writer.write_buffer(0, gpuSceneDataBuffer.buffer, sizeof(render::GPU_scene_data), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
// 	writer.update_set(m_device, m_global_descriptor);




// and for my meshes I bind the descriptor like this:
// 	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, loc_pipeline->layout, 0, 1, &m_global_descriptor, 0, nullptr);
