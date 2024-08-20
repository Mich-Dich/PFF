#pragma once

namespace PFF::render::vulkan {

    namespace util {

        // @brief Consider abstracting the path to work from a folder set in a config file, and only select the shader (e.g: combute.comp => internally looking in correct folder) (platform agnostic)
        bool load_shader_module(const char* filePath, VkDevice device, VkShaderModule* outShaderModule);
    }

    class pipeline_builder {
    public:

        pipeline_builder() { clear(); }

        void clear();
        VkPipeline build(VkDevice device);

        // setters
        pipeline_builder& set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        pipeline_builder& set_input_topology(VkPrimitiveTopology topology);
        pipeline_builder& set_polygon_mode(VkPolygonMode mode, float line_width = 1.f);
        pipeline_builder& set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace);
        pipeline_builder& set_multisampling_none();
        pipeline_builder& set_color_attachment_format(VkFormat format);
        pipeline_builder& set_depth_format(VkFormat format);
        pipeline_builder& set_pipeline_layout(VkPipelineLayout pipeline_layout) { p_pipeline_layout = pipeline_layout; return *this; }

        // feature
        pipeline_builder& disable_blending();
        pipeline_builder& enable_blending_additive();
        pipeline_builder& enable_blending_alphablend();
        pipeline_builder& disable_depthtest();
        pipeline_builder& enable_depthtest(bool depthWriteEnable, VkCompareOp op);

    private:

        std::vector<VkPipelineShaderStageCreateInfo>    p_shader_stages;
        VkPipelineInputAssemblyStateCreateInfo          p_input_assembly_CI;
        VkPipelineRasterizationStateCreateInfo          p_rasterizer_CI;
        VkPipelineMultisampleStateCreateInfo            p_multisampling_CI;
        VkPipelineLayout                                p_pipeline_layout;
        VkPipelineDepthStencilStateCreateInfo           p_depth_stencil_CI;
        VkPipelineRenderingCreateInfo                   p_render_info_CI;
        VkFormat                                        p_color_attachmentformat;

        VkPipelineColorBlendAttachmentState             m_color_blend_attachment_CI;
    };

}
