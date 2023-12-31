#pragma once

#include "util/util.h"


namespace PFF {

	// forwand declaration
	class vk_device;

	struct pipeline_config_info {

		VkRect2D scissor;
		VkViewport viewport;
		VkPipelineInputAssemblyStateCreateInfo input_assembly_CI;
		VkPipelineRasterizationStateCreateInfo rasterization_CI;
		VkPipelineMultisampleStateCreateInfo multisample_CI;
		VkPipelineColorBlendAttachmentState color_blend_attachment;
		VkPipelineColorBlendStateCreateInfo color_blend_CI;
		VkPipelineDepthStencilStateCreateInfo depth_stencil_CI;
		VkPipelineLayout pipeline_layout = nullptr;
		VkRenderPass render_pass = nullptr;
		u32 subpass = 0;

		pipeline_config_info(u32 width, u32 height, VkPipelineLayout in_pipeline_layout, VkRenderPass in_render_pass, u32 in_subpass);
	};

	class vk_pipeline {

	public:

		vk_pipeline(
			std::shared_ptr<vk_device> device,
			const pipeline_config_info config, 
			const std::string& vert_file_path, 
			const std::string& frag_file_path);
		
		~vk_pipeline();

		DELETE_COPY(vk_pipeline)

		void bind_commnad_buffers(VkCommandBuffer command_buffer);

	private:

		void create_graphics_pipeline(
			const pipeline_config_info config, 
			const std::string& vert_file_path, 
			const std::string& frag_file_path);
		
		void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);

		std::shared_ptr<vk_device> m_device;

		VkPipeline m_graphics_pipeline;
		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;

	};

}