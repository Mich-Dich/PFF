#pragma once

#include <vulkan/vulkan.h>

namespace PFF {

	// forwand declaration
	class vk_device;
	
	/*
	// ============================================ NEW VERSION ============================================ (doesnt need .cpp file)
	struct graphics_pipeline_input {
		VkDevice device;
		std::string vertex_file_path;
		std::string fragment_file_path;
		VkExtent2D swapchain_extend;
		VkFormat swapchain_image_format;
	};

	struct graphics_pipeline_output {
		VkPipelineLayout layout;
		VkRenderPass render_pass;
		VkPipeline pipeline;
	};

	void make_pipeline_layout(VkDevice device, VkPipelineLayout layout) {

		VkPipelineLayoutCreateInfo layout_CI{};

	}
	
	// ============================================ OLD VERSION ============================================

	*/

	struct pipeline_config_info {

		VkPipelineViewportStateCreateInfo		viewport_CI{};
		VkPipelineInputAssemblyStateCreateInfo	input_assembly_CI{};
		VkPipelineRasterizationStateCreateInfo	rasterization_CI{};
		VkPipelineMultisampleStateCreateInfo	multisample_CI{};
		VkPipelineColorBlendAttachmentState		color_blend_attachment{};
		VkPipelineColorBlendStateCreateInfo		color_blend_CI{};
		VkPipelineDepthStencilStateCreateInfo	depth_stencil_CI{};
		std::vector<VkDynamicState>				dynamic_states_enables{};
		VkPipelineDynamicStateCreateInfo		dynamic_state_CI{};
		VkPipelineLayout						pipeline_layout = nullptr;
		VkRenderPass							render_pass = nullptr;
		u32										subpass = 0;

		pipeline_config_info();

		// DELETE_COPY(pipeline_config_info);
	};

	class vk_pipeline {

	public:

		vk_pipeline(ref<vk_device> device, const pipeline_config_info& config, const std::string& vert_file_path, const std::string& frag_file_path);
		~vk_pipeline();

		DELETE_COPY(vk_pipeline);

		FORCEINLINE u32 get_subpass() const { return m_subpass; }

		void bind_commnad_buffers(VkCommandBuffer command_buffer);

	private:

		void create_graphics_pipeline(const pipeline_config_info& config, const std::string& vert_file_path, const std::string& frag_file_path);

		ref<vk_device> m_device;

		u32 m_subpass = 0;
		VkPipeline m_graphics_pipeline;
		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;

	};

}