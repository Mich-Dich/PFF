
#include "util/pffpch.h"

#include "engine/render/vk_device.h"
#include "engine/geometry/basic_mesh.h"

#include "vk_pipeline.h"


namespace PFF {

	pipeline_config_info::pipeline_config_info() {

		PFF_PROFILE_FUNCTION();

		/*
		//pipeline_config_info CI{};
		pipeline_layout = in_pipeline_layout;
		render_pass = in_render_pass;
		subpass = in_subpass;*/

		input_assembly_CI = VkPipelineInputAssemblyStateCreateInfo{};
		input_assembly_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly_CI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly_CI.primitiveRestartEnable = VK_FALSE;

		viewport_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_CI.viewportCount = 1;
		viewport_CI.pViewports = nullptr;
		viewport_CI.scissorCount = 1;
		viewport_CI.pScissors = nullptr;

		rasterization_CI = VkPipelineRasterizationStateCreateInfo{};
		rasterization_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_CI.depthClampEnable = VK_FALSE;
		rasterization_CI.rasterizerDiscardEnable = VK_FALSE;
		rasterization_CI.polygonMode = VK_POLYGON_MODE_FILL;
		rasterization_CI.lineWidth = 1.0f;
		rasterization_CI.cullMode = VK_CULL_MODE_NONE;
		rasterization_CI.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterization_CI.depthBiasEnable = VK_FALSE;
		rasterization_CI.depthBiasConstantFactor = 0.0f;  // Optional
		rasterization_CI.depthBiasClamp = 0.0f;           // Optional
		rasterization_CI.depthBiasSlopeFactor = 0.0f;     // Optional

		multisample_CI = VkPipelineMultisampleStateCreateInfo{};
		multisample_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample_CI.sampleShadingEnable = VK_FALSE;
		multisample_CI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisample_CI.minSampleShading = 1.0f;           // Optional
		multisample_CI.pSampleMask = nullptr;             // Optional
		multisample_CI.alphaToCoverageEnable = VK_FALSE;  // Optional
		multisample_CI.alphaToOneEnable = VK_FALSE;       // Optional

		color_blend_attachment = VkPipelineColorBlendAttachmentState{};
		color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		color_blend_attachment.blendEnable = VK_FALSE;
		color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		color_blend_CI = VkPipelineColorBlendStateCreateInfo{};
		color_blend_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_CI.logicOpEnable = VK_FALSE;
		color_blend_CI.logicOp = VK_LOGIC_OP_COPY;  // Optional
		color_blend_CI.attachmentCount = 1;
		color_blend_CI.pAttachments = &color_blend_attachment;
		color_blend_CI.blendConstants[0] = 0.0f;  // Optional
		color_blend_CI.blendConstants[1] = 0.0f;  // Optional
		color_blend_CI.blendConstants[2] = 0.0f;  // Optional
		color_blend_CI.blendConstants[3] = 0.0f;  // Optional

		depth_stencil_CI = VkPipelineDepthStencilStateCreateInfo{};
		depth_stencil_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil_CI.depthTestEnable = VK_TRUE;
		depth_stencil_CI.depthWriteEnable = VK_TRUE;
		depth_stencil_CI.depthCompareOp = VK_COMPARE_OP_LESS;
		depth_stencil_CI.depthBoundsTestEnable = VK_FALSE;
		depth_stencil_CI.minDepthBounds = 0.0f;  // Optional
		depth_stencil_CI.maxDepthBounds = 1.0f;  // Optional
		depth_stencil_CI.stencilTestEnable = VK_FALSE;
		depth_stencil_CI.front = {};  // Optional
		depth_stencil_CI.back = {};   // Optional

		dynamic_states_enables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		dynamic_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state_CI.pDynamicStates = dynamic_states_enables.data();
		dynamic_state_CI.dynamicStateCount = static_cast<u32>(dynamic_states_enables.size());
		dynamic_state_CI.flags = 0;
	}



	//
	vk_pipeline::vk_pipeline(ref<vk_device> device, const pipeline_config_info& config, const std::string& vert_file_path, const std::string& frag_file_path)
		:m_device{ device } {

		PFF_PROFILE_FUNCTION();

		LOG(Fatal, "Creating Pipeline");
		create_graphics_pipeline(config, vert_file_path, frag_file_path);
	}

	//
	vk_pipeline::~vk_pipeline() {

		PFF_PROFILE_FUNCTION();

		vkDestroyShaderModule(m_device->get_device(), m_vert_shader_module, nullptr);
		vkDestroyShaderModule(m_device->get_device(), m_frag_shader_module, nullptr);
		vkDestroyPipeline(m_device->get_device(), m_graphics_pipeline, nullptr);

		m_device.reset();
		LOG(Fatal, "shutdown");
	}

	void vk_pipeline::bind_commnad_buffers(VkCommandBuffer command_buffer) {

		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);
	}

	void vk_pipeline::create_graphics_pipeline(const pipeline_config_info& config, const std::string& vert_file_path, const std::string& frag_file_path) {

		PFF_PROFILE_FUNCTION();

		/*
		auto config = default_pipline_config_info(config.scissor.extent.width, config.scissor.extent.height);
		config.render_pass = config.render_pass;
		config.pipeline_layout = config.pipeline_layout;
		*/
		CORE_ASSERT_S(config.pipeline_layout != VK_NULL_HANDLE);
		CORE_ASSERT_S(config.render_pass != VK_NULL_HANDLE);

		std::vector<char> shader_code;
		CORE_ASSERT_S(io_handler::read_file(vert_file_path, shader_code));
		create_shader_module(shader_code, &m_vert_shader_module);

		CORE_ASSERT_S(io_handler::read_file(frag_file_path, shader_code));
		create_shader_module(shader_code, &m_frag_shader_module);

		VkPipelineShaderStageCreateInfo shader_stage_CI[2]{};
		shader_stage_CI[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader_stage_CI[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shader_stage_CI[0].module = m_vert_shader_module;
		shader_stage_CI[0].pName = "main";
		shader_stage_CI[0].flags = 0;
		shader_stage_CI[0].pNext = nullptr;
		shader_stage_CI[0].pSpecializationInfo = nullptr;

		shader_stage_CI[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader_stage_CI[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shader_stage_CI[1].module = m_frag_shader_module;
		shader_stage_CI[1].pName = "main";
		shader_stage_CI[1].flags = 0;
		shader_stage_CI[1].pNext = nullptr;
		shader_stage_CI[1].pSpecializationInfo = nullptr;

		auto attribute_desc = basic_mesh::vertex::get_attribute_descriptions();
		auto binding_desc = basic_mesh::vertex::get_binding_descriptions();

		VkPipelineVertexInputStateCreateInfo vert_input_SCT{};
		vert_input_SCT.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vert_input_SCT.vertexAttributeDescriptionCount = static_cast<u32>(attribute_desc.size());
		vert_input_SCT.vertexBindingDescriptionCount = static_cast<u32>(binding_desc.size());
		vert_input_SCT.pVertexAttributeDescriptions = attribute_desc.data();
		vert_input_SCT.pVertexBindingDescriptions = binding_desc.data();

		VkGraphicsPipelineCreateInfo pipeline_CI{};
		pipeline_CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_CI.stageCount = 2;
		pipeline_CI.pStages = shader_stage_CI;
		pipeline_CI.pVertexInputState = &vert_input_SCT;
		pipeline_CI.pInputAssemblyState = &config.input_assembly_CI;
		pipeline_CI.pViewportState = &config.viewport_CI;
		pipeline_CI.pRasterizationState = &config.rasterization_CI;
		pipeline_CI.pMultisampleState = &config.multisample_CI;
		pipeline_CI.pColorBlendState = &config.color_blend_CI;
		pipeline_CI.pDynamicState = &config.dynamic_state_CI;
		pipeline_CI.pDepthStencilState = &config.depth_stencil_CI;

		pipeline_CI.layout = config.pipeline_layout;
		pipeline_CI.renderPass = config.render_pass;
		pipeline_CI.subpass = config.subpass;
		m_subpass = config.subpass;

		pipeline_CI.basePipelineHandle = VK_NULL_HANDLE;  // Optional
		pipeline_CI.basePipelineIndex = -1;               // Optional

		CORE_ASSERT_S(&pipeline_CI != nullptr);
		CORE_ASSERT_S(&m_graphics_pipeline != nullptr);

		CORE_ASSERT(vkCreateGraphicsPipelines(m_device->get_device(), VK_NULL_HANDLE, 1, &pipeline_CI, nullptr, &m_graphics_pipeline) == VK_SUCCESS,
			"Created graphics pipeline", "Failed to create graphics pipeline");
	}

	void vk_pipeline::create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module) {

		PFF_PROFILE_FUNCTION();

		VkShaderModuleCreateInfo SM_CI{};
		SM_CI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		SM_CI.codeSize = code.size();
		SM_CI.pCode = reinterpret_cast<const uint32_t*>(code.data());

		CORE_ASSERT_S(vkCreateShaderModule(m_device->get_device(), &SM_CI, nullptr, shader_module) == VK_SUCCESS);

	}

}
