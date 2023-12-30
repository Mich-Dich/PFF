#pragma once

#include "util/util.h"


namespace PFF {

	// forwand declaration
	class vk_device;

	struct pipeline_cinfig_info {

		VkRect2D scissor;
		VkViewport viewport;
		VkPipelineInputAssemblyStateCreateInfo input_assembly_CI;
		VkPipelineRasterizationStateCreateInfo rasterization_CI;
		VkPipelineMultisampleStateCreateInfo multisample_CI;
		VkPipelineColorBlendAttachmentState color_blend_attachment;
		VkPipelineColorBlendStateCreateInfo color_blend_CI;
		VkPipelineDepthStencilStateCreateInfo depth_stencil_CI;
		VkPipelineLayout pipeline_layout = nullptr;
		VkRenderPass render_pPass = nullptr;
		uint32_t subpass = 0;
	};

	class vk_pipeline {

	public:

		vk_pipeline(
			std::shared_ptr<vk_device> device,
			const pipeline_cinfig_info config, 
			const std::string& vert_file_path, 
			const std::string& frag_file_path);
		
		~vk_pipeline();

		vk_pipeline(const vk_pipeline&) = delete;
		vk_pipeline& operator=(const vk_pipeline&) = delete;

		static pipeline_cinfig_info default_pipline_config_info(u32 width, u32 height);

	private:

		void create_graphics_pipeline(
			const pipeline_cinfig_info config, 
			const std::string& vert_file_path, 
			const std::string& frag_file_path);
		
		void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);

		std::shared_ptr<vk_device> m_device;
		VkPipeline m_graphics_pipeline;
		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;

	};

}