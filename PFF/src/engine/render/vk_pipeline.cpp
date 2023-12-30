
#include "util/io_handler.h"
#include "engine/render/vk_device.h"

#include "vk_pipeline.h"


namespace PFF {

	//
	vk_pipeline::vk_pipeline(std::shared_ptr<vk_device> device, const pipeline_cinfig_info config, const std::string& vert_file_path, const std::string& frag_file_path)
		:m_device{ device } {

		create_graphics_pipeline(config, vert_file_path, frag_file_path);
	}

	//
	vk_pipeline::~vk_pipeline() {

		vkDestroyShaderModule(m_device->get_device(), m_vert_shader_module, nullptr);
		vkDestroyShaderModule(m_device->get_device(), m_frag_shader_module, nullptr);
		vkDestroyPipeline(m_device->get_device(), m_graphics_pipeline, nullptr);


	}

	pipeline_cinfig_info vk_pipeline::default_pipline_config_info(u32 width, u32 height) {

		pipeline_cinfig_info CI{};
		CI.input_assembly_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		CI.input_assembly_CI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		CI.input_assembly_CI.primitiveRestartEnable = VK_FALSE;

		CI.viewport.x = 0.0f;
		CI.viewport.y = 0.0f;
		CI.viewport.width = static_cast<float>(width);
		CI.viewport.height = static_cast<float>(height);
		CI.viewport.minDepth = 0.0f;
		CI.viewport.maxDepth = 1.0f;

		CI.scissor.offset = { 0, 0 };
		CI.scissor.extent = { width, height };

		CI.rasterization_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		CI.rasterization_CI.depthClampEnable = VK_FALSE;
		CI.rasterization_CI.rasterizerDiscardEnable = VK_FALSE;
		CI.rasterization_CI.polygonMode = VK_POLYGON_MODE_FILL;
		CI.rasterization_CI.lineWidth = 1.0f;
		CI.rasterization_CI.cullMode = VK_CULL_MODE_NONE;
		CI.rasterization_CI.frontFace = VK_FRONT_FACE_CLOCKWISE;
		CI.rasterization_CI.depthBiasEnable = VK_FALSE;
		CI.rasterization_CI.depthBiasConstantFactor = 0.0f;  // Optional
		CI.rasterization_CI.depthBiasClamp = 0.0f;           // Optional
		CI.rasterization_CI.depthBiasSlopeFactor = 0.0f;     // Optional

		CI.multisample_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		CI.multisample_CI.sampleShadingEnable = VK_FALSE;
		CI.multisample_CI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		CI.multisample_CI.minSampleShading = 1.0f;           // Optional
		CI.multisample_CI.pSampleMask = nullptr;             // Optional
		CI.multisample_CI.alphaToCoverageEnable = VK_FALSE;  // Optional
		CI.multisample_CI.alphaToOneEnable = VK_FALSE;       // Optional

		CI.color_blend_attachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		CI.color_blend_attachment.blendEnable = VK_FALSE;
		CI.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		CI.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		CI.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		CI.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		CI.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		CI.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		CI.color_blend_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		CI.color_blend_CI.logicOpEnable = VK_FALSE;
		CI.color_blend_CI.logicOp = VK_LOGIC_OP_COPY;  // Optional
		CI.color_blend_CI.attachmentCount = 1;
		CI.color_blend_CI.pAttachments = &CI.color_blend_attachment;
		CI.color_blend_CI.blendConstants[0] = 0.0f;  // Optional
		CI.color_blend_CI.blendConstants[1] = 0.0f;  // Optional
		CI.color_blend_CI.blendConstants[2] = 0.0f;  // Optional
		CI.color_blend_CI.blendConstants[3] = 0.0f;  // Optional

		CI.depth_stencil_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		CI.depth_stencil_CI.depthTestEnable = VK_TRUE;
		CI.depth_stencil_CI.depthWriteEnable = VK_TRUE;
		CI.depth_stencil_CI.depthCompareOp = VK_COMPARE_OP_LESS;
		CI.depth_stencil_CI.depthBoundsTestEnable = VK_FALSE;
		CI.depth_stencil_CI.minDepthBounds = 0.0f;  // Optional
		CI.depth_stencil_CI.maxDepthBounds = 1.0f;  // Optional
		CI.depth_stencil_CI.stencilTestEnable = VK_FALSE;
		CI.depth_stencil_CI.front = {};  // Optional
		CI.depth_stencil_CI.back = {};   // Optional

		return CI;
	}

	void vk_pipeline::create_graphics_pipeline(const pipeline_cinfig_info config, const std::string& vert_file_path, const std::string& frag_file_path) {

		CORE_ASSERT_S(config.pipeline_layout != VK_NULL_HANDLE);
		CORE_ASSERT_S(config.render_pPass != VK_NULL_HANDLE);

		auto vertCode = io_handler::read_file(vert_file_path);
		auto fragCode = io_handler::read_file(frag_file_path);

		create_shader_module(vertCode, &m_vert_shader_module);
		create_shader_module(fragCode, &m_frag_shader_module);

		VkPipelineShaderStageCreateInfo shader_stage_CI[2];
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

		VkPipelineVertexInputStateCreateInfo vert_input_SCT;
		vert_input_SCT.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vert_input_SCT.vertexAttributeDescriptionCount = 0;
		vert_input_SCT.vertexBindingDescriptionCount = 0;
		vert_input_SCT.pVertexAttributeDescriptions = nullptr;
		vert_input_SCT.pVertexBindingDescriptions = nullptr;

		VkPipelineViewportStateCreateInfo viewport_CI{};
		viewport_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_CI.viewportCount = 1;
		viewport_CI.pViewports = &config.viewport;
		viewport_CI.scissorCount = 1;
		viewport_CI.pScissors = &config.scissor;

		VkGraphicsPipelineCreateInfo pipeline_CI;
		pipeline_CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_CI.stageCount = 2;
		pipeline_CI.pStages = shader_stage_CI;
		pipeline_CI.pVertexInputState = &vert_input_SCT;
		pipeline_CI.pInputAssemblyState = &config.input_assembly_CI;
		pipeline_CI.pViewportState = &viewport_CI;
		pipeline_CI.pRasterizationState = &config.rasterization_CI;
		pipeline_CI.pMultisampleState = &config.multisample_CI;
		pipeline_CI.pColorBlendState = &config.color_blend_CI;
		pipeline_CI.pDynamicState = nullptr;  // Optional
		pipeline_CI.pDepthStencilState = &config.depth_stencil_CI;

		pipeline_CI.layout = config.pipeline_layout;
		pipeline_CI.renderPass = config.render_pPass;
		pipeline_CI.subpass = config.subpass;

		pipeline_CI.basePipelineHandle = VK_NULL_HANDLE;  // Optional
		pipeline_CI.basePipelineIndex = -1;               // Optional

		CORE_ASSERT_S(vkCreateGraphicsPipelines(m_device->get_device(), VK_NULL_HANDLE, 1, &pipeline_CI, nullptr, &m_graphics_pipeline) == VK_SUCCESS)

	}

	void vk_pipeline::create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module) {

		VkShaderModuleCreateInfo SM_CI{};
		SM_CI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		SM_CI.codeSize = code.size();
		SM_CI.pCode = reinterpret_cast<const uint32_t*>(code.data());

		CORE_ASSERT_S(vkCreateShaderModule(m_device->get_device(), &SM_CI, nullptr, shader_module) == VK_SUCCESS);

	}

}
