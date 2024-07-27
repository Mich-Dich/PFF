
#include "util/pffpch.h"

#include "application.h"
#include "vulkan/vk_pipeline.h"
#include "vulkan/vk_initializers.h"

#include "material.h"

namespace PFF {

	void material::build_pipelines() {

		VkShaderModule meshFragShader;
		CORE_ASSERT(render::vulkan::util::load_shader_module("../PFF/shaders/mesh.frag.spv", GET_RENDERER.get_device(), &meshFragShader), "", "Error when building the triangle fragment shader module");

		VkShaderModule meshVertexShader;
		CORE_ASSERT(render::vulkan::util::load_shader_module("../PFF/shaders/mesh.vert.spv", GET_RENDERER.get_device(), &meshVertexShader), "", "Error when building the triangle vertex shader module");

		VkPushConstantRange matrix_range{};
		matrix_range.offset = 0;
		matrix_range.size = sizeof(GPU_draw_push_constants);
		matrix_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		render::vulkan::descriptor_layout_builder layout_builder;
		layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		layout_builder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		layout_builder.add_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		material_layout = layout_builder.build(GET_RENDERER.get_device(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

		VkDescriptorSetLayout layouts[] = { GET_RENDERER.get_gpu_scene_data_descriptor_layout(), material_layout};

		VkPipelineLayoutCreateInfo mesh_layout_info = render::vulkan::init::pipeline_layout_create_info();
		mesh_layout_info.setLayoutCount = 2;
		mesh_layout_info.pSetLayouts = layouts;
		mesh_layout_info.pPushConstantRanges = &matrix_range;
		mesh_layout_info.pushConstantRangeCount = 1;

		VkPipelineLayout newLayout;
		VK_CHECK_S(vkCreatePipelineLayout(GET_RENDERER.get_device(), &mesh_layout_info, nullptr, &newLayout));

		opaque_pipeline.layout = newLayout;
		transparent_pipeline.layout = newLayout;

		// build the stage-create-info for both vertex and fragment stages. This lets
		// the pipeline know the shader modules per stage
		render::vulkan::pipeline_builder pipeline_builder;
		pipeline_builder.set_shaders(meshVertexShader, meshFragShader);
		pipeline_builder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		pipeline_builder.set_polygon_mode(VK_POLYGON_MODE_FILL);
		pipeline_builder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
		pipeline_builder.set_multisampling_none();
		pipeline_builder.disable_blending();
		pipeline_builder.enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
		pipeline_builder.set_color_attachment_format(GET_RENDERER.get_draw_image_pointer()->get_image_format());
		pipeline_builder.set_depth_format(GET_RENDERER.get_depth_image_pointer()->get_image_format());
		pipeline_builder.set_pipeline_layout(newLayout);												// use the triangle layout we 
		opaque_pipeline.pipeline = pipeline_builder.build(GET_RENDERER.get_device());		// finally build the pipeline

		// create the transparent variant
		pipeline_builder.enable_blending_additive();
		pipeline_builder.enable_depthtest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);
		transparent_pipeline.pipeline = pipeline_builder.build(GET_RENDERER.get_device());

		vkDestroyShaderModule(GET_RENDERER.get_device(), meshFragShader, nullptr);
		vkDestroyShaderModule(GET_RENDERER.get_device(), meshVertexShader, nullptr);
	}

	void material::release_resources() {

		VkDevice device = GET_RENDERER.get_device();

		vkDestroyDescriptorSetLayout(device, material_layout, nullptr);
		vkDestroyPipelineLayout(device, transparent_pipeline.layout, nullptr);
		vkDestroyPipeline(device, transparent_pipeline.pipeline, nullptr);
		vkDestroyPipeline(device, opaque_pipeline.pipeline, nullptr);
	}

	material_instance material::write_material(material_pass pass, const material_resources& resources, render::vulkan::descriptor_allocator_growable& descriptor_allocator) {

		material_instance loc_mat_inst;
		loc_mat_inst.pass_type = pass;
		loc_mat_inst.pipeline = (pass == material_pass::transparent) ? &transparent_pipeline : &opaque_pipeline;
		loc_mat_inst.material_set = descriptor_allocator.allocate(GET_RENDERER.get_device(), material_layout);

		writer.clear();
		writer.write_buffer(0, resources.data_buffer, sizeof(material_constants), resources.data_buffer_offset, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		writer.write_image(1, resources.color_image->get_image_view(), resources.color_sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		writer.write_image(2, resources.metal_rough_image->get_image_view(), resources.metal_rough_sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		writer.update_set(GET_RENDERER.get_device(), loc_mat_inst.material_set);

		return loc_mat_inst;
	}

}
