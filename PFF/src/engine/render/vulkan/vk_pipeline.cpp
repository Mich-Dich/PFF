
#include <util/pffpch.h>

#include "vk_types.h"
#include "vk_initializers.h"

#include "vk_pipeline.h"

namespace PFF::render::vulkan {

    namespace util {

        bool load_shader_module(const std::filesystem::path& filePath, VkDevice device, VkShaderModule* outShaderModule) {

            // open the file. With cursor at the end
            std::ifstream file(filePath, std::ios::ate | std::ios::binary);

            if (!file.is_open()) {
                LOG(Error, "Failed to open shader file: " << filePath);
                return false;
            }

            size_t fileSize = (size_t)file.tellg();
            std::vector<u32> buffer(fileSize / sizeof(u32));
            file.seekg(0);
            file.read((char*)buffer.data(), fileSize);
            file.close();

            // create a new shader module, using the buffer we loaded
            VkShaderModuleCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.pNext = nullptr;

            // codeSize has to be in bytes, so multply the ints in the buffer by size of
            // int to know the real size of the buffer
            createInfo.codeSize = buffer.size() * sizeof(u32);
            createInfo.pCode = buffer.data();

            VkShaderModule shaderModule;
            VALIDATE((vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) == VK_SUCCESS), return false, "", "Failed to create shader module from file: " << filePath);

            *outShaderModule = shaderModule;
            LOG(Trace, "Successfully loaded and created shader module [" << PFF::util::extract_path_from_directory(filePath, "PFF") << "]");
            return true;
        }
    }

    void pipeline_builder::clear() {
        //clear all of the structs we need back to 0 with their correct stype

        p_input_assembly_CI = {};
        p_input_assembly_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

        p_rasterizer_CI = {};
        p_rasterizer_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

        m_color_blend_attachment_CI = {};

        p_multisampling_CI = {};
        p_multisampling_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

        p_pipeline_layout = {};

        p_depth_stencil_CI = {};
        p_depth_stencil_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

        p_render_info_CI = {};
        p_render_info_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

        p_shader_stages.clear();
    }

    VkPipeline pipeline_builder::build(VkDevice device) {

        // make viewport state from our stored viewport and scissor.
        // at the moment not supportingmultiple viewports or scissors
        VkPipelineViewportStateCreateInfo viewport_state_CI = {};
        viewport_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_CI.pNext = nullptr;

        viewport_state_CI.viewportCount = 1;
        viewport_state_CI.scissorCount = 1;

        // setup dummy color blending. Not using transparent objects yet
        // the blending is just "no blend", but write to the color attachment
        VkPipelineColorBlendStateCreateInfo color_blending_CI = {};
        color_blending_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending_CI.pNext = nullptr;

        color_blending_CI.logicOpEnable = VK_FALSE;
        color_blending_CI.logicOp = VK_LOGIC_OP_COPY;
        color_blending_CI.attachmentCount = 1;
        color_blending_CI.pAttachments = &m_color_blend_attachment_CI;

        // completely clear VertexInputStateCreateInfo, as we have no need for it
        VkPipelineVertexInputStateCreateInfo vertex_input_CI = {};
        vertex_input_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkDynamicState state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamic_state_CI = {};
        dynamic_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state_CI.pDynamicStates = &state[0];
        dynamic_state_CI.dynamicStateCount = 2;

        // build the actual pipeline
        // we now use all of the info structs we have been writing into into this one to create the pipeline
        VkGraphicsPipelineCreateInfo graphics_pipeline_CI = {};
        graphics_pipeline_CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphics_pipeline_CI.pNext = &p_render_info_CI;  //connect the renderInfo to the pNext extension mechanism

        graphics_pipeline_CI.stageCount = (uint32_t)p_shader_stages.size();
        graphics_pipeline_CI.pStages = p_shader_stages.data();
        graphics_pipeline_CI.pVertexInputState = &vertex_input_CI;
        graphics_pipeline_CI.pInputAssemblyState = &p_input_assembly_CI;
        graphics_pipeline_CI.pViewportState = &viewport_state_CI;
        graphics_pipeline_CI.pRasterizationState = &p_rasterizer_CI;
        graphics_pipeline_CI.pMultisampleState = &p_multisampling_CI;
        graphics_pipeline_CI.pColorBlendState = &color_blending_CI;
        graphics_pipeline_CI.pDepthStencilState = &p_depth_stencil_CI;
        graphics_pipeline_CI.layout = p_pipeline_layout;

        graphics_pipeline_CI.pDynamicState = &dynamic_state_CI;

        VkPipeline newPipeline;
        VALIDATE(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphics_pipeline_CI, nullptr, &newPipeline) == VK_SUCCESS, return VK_NULL_HANDLE, "", "failed to create pipeline");
        return newPipeline;
    }

    // ========================================================================================= setters =========================================================================================

    pipeline_builder& pipeline_builder::set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader) {

        p_shader_stages.clear();
        p_shader_stages.push_back(init::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertexShader));
        p_shader_stages.push_back(init::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
        return *this;
    }

    pipeline_builder& pipeline_builder::set_input_topology(VkPrimitiveTopology topology) {

        p_input_assembly_CI.topology = topology;
        p_input_assembly_CI.primitiveRestartEnable = VK_FALSE;      // NOT used for now => enables restart for triangle_strips and line_strips
        return *this;
    }

    pipeline_builder& pipeline_builder::set_polygon_mode(VkPolygonMode mode, float line_width) {

        p_rasterizer_CI.polygonMode = mode;
        p_rasterizer_CI.lineWidth = line_width;
        return *this;
    }

    pipeline_builder& pipeline_builder::set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace) {

        p_rasterizer_CI.cullMode = cullMode;
        p_rasterizer_CI.frontFace = frontFace;
        return *this;
    }

    pipeline_builder& pipeline_builder::set_multisampling_none() {

        p_multisampling_CI.sampleShadingEnable = VK_FALSE;
        p_multisampling_CI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;    // multisampling defaulted to no multisampling (1 sample per pixel)
        p_multisampling_CI.minSampleShading = 1.0f;
        p_multisampling_CI.pSampleMask = nullptr;
        p_multisampling_CI.alphaToCoverageEnable = VK_FALSE;                //no alpha to coverage either
        p_multisampling_CI.alphaToOneEnable = VK_FALSE;
        return *this;
    }

    pipeline_builder& pipeline_builder::set_color_attachment_format(VkFormat format) {

        p_color_attachmentformat = format;
        p_render_info_CI.colorAttachmentCount = 1;      // connect the format to the renderInfo  structure
        p_render_info_CI.pColorAttachmentFormats = &p_color_attachmentformat;
        return *this;
    }

    pipeline_builder& pipeline_builder::set_depth_format(VkFormat format) {

        p_render_info_CI.depthAttachmentFormat = format;
        return *this;
    }

    // ========================================================================================= feature =========================================================================================

    pipeline_builder& pipeline_builder::disable_culling() {

        p_rasterizer_CI.cullMode = VK_CULL_MODE_NONE;
        return *this;
    }

    pipeline_builder& pipeline_builder::disable_blending() {

        m_color_blend_attachment_CI.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;   //default write mask
        m_color_blend_attachment_CI.blendEnable = VK_FALSE;                 //no blending
        return *this;
    }

    pipeline_builder& pipeline_builder::enable_blending_additive() {

        m_color_blend_attachment_CI.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        m_color_blend_attachment_CI.blendEnable = VK_TRUE;
        m_color_blend_attachment_CI.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        m_color_blend_attachment_CI.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
        m_color_blend_attachment_CI.colorBlendOp = VK_BLEND_OP_ADD;
        m_color_blend_attachment_CI.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        m_color_blend_attachment_CI.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        m_color_blend_attachment_CI.alphaBlendOp = VK_BLEND_OP_ADD;
        return *this;
    }

    pipeline_builder& pipeline_builder::enable_blending_alphablend() {

        m_color_blend_attachment_CI.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        m_color_blend_attachment_CI.blendEnable = VK_TRUE;
        m_color_blend_attachment_CI.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        m_color_blend_attachment_CI.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
        m_color_blend_attachment_CI.colorBlendOp = VK_BLEND_OP_ADD;
        m_color_blend_attachment_CI.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        m_color_blend_attachment_CI.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        m_color_blend_attachment_CI.alphaBlendOp = VK_BLEND_OP_ADD;
        return *this;
    }

    pipeline_builder& pipeline_builder::disable_depthtest() {

        p_depth_stencil_CI.depthTestEnable = VK_FALSE;
        p_depth_stencil_CI.depthWriteEnable = VK_FALSE;
        p_depth_stencil_CI.depthCompareOp = VK_COMPARE_OP_NEVER;
        p_depth_stencil_CI.depthBoundsTestEnable = VK_FALSE;
        p_depth_stencil_CI.stencilTestEnable = VK_FALSE;
        p_depth_stencil_CI.front = {};
        p_depth_stencil_CI.back = {};
        p_depth_stencil_CI.minDepthBounds = 0.f;
        p_depth_stencil_CI.maxDepthBounds = 1.f;
        return *this;
    }


    pipeline_builder& pipeline_builder::enable_depthtest(bool depthWriteEnable, VkCompareOp op) {

        p_depth_stencil_CI.depthTestEnable = VK_TRUE;
        p_depth_stencil_CI.depthWriteEnable = depthWriteEnable;
        p_depth_stencil_CI.depthCompareOp = op;
        p_depth_stencil_CI.depthBoundsTestEnable = VK_FALSE;
        p_depth_stencil_CI.stencilTestEnable = VK_FALSE;
        p_depth_stencil_CI.front = {};
        p_depth_stencil_CI.back = {};
        p_depth_stencil_CI.minDepthBounds = 0.f;
        p_depth_stencil_CI.maxDepthBounds = 1.f;
        return *this;
    }
}
