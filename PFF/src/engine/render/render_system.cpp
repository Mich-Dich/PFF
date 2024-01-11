
#include "util/pffpch.h"

#include <vulkan/vulkan.h>

#include "engine/platform/pff_window.h"
#include "engine/render/vk_device.h"
#include "engine/render/vk_pipeline.h"
#include "engine/render/vk_swapchain.h"
#include "engine/render/renderer.h"

#include "engine/geometry/basic_mesh.h"
#include "engine/game_objects/game_object.h"

/*#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
*/
#include "render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace PFF {

	struct simple_push_constant_data {
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};


	render_system::render_system(std::shared_ptr<vk_device> device, VkRenderPass renderPass, VkDescriptorSetLayout descriptor_set_layout)
	 : m_device( device ) {

		create_pipeline_layout(descriptor_set_layout);
		create_pipeline(renderPass);
		CORE_LOG(Trace, "render_system started");
	}

	render_system::~render_system() {

		vkDestroyPipelineLayout(m_device->get_device(), m_pipeline_layout, nullptr);
	}

	void render_system::render_game_objects(f32 delta_time, VkCommandBuffer command_buffer, std::vector<game_object>& game_objects) {

		m_vk_pipeline->bind_commnad_buffers(command_buffer);

		for (auto& obj : game_objects) {
			
			simple_push_constant_data PCD{};
			PCD.offset = obj.transform_2D.translation;
			PCD.color = obj.color;
			PCD.transform = obj.transform_2D.mat2();

			vkCmdPushConstants(command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(simple_push_constant_data), &PCD);

			obj.mesh->bind(command_buffer);
			obj.mesh->draw(command_buffer);
		}
	}

	void render_system::create_pipeline_layout(VkDescriptorSetLayout descriptor_set_layout) {

		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(simple_push_constant_data);

		VkPipelineLayoutCreateInfo pipeline_layout_CI{};
		pipeline_layout_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_CI.setLayoutCount = 0;
		pipeline_layout_CI.pSetLayouts = &descriptor_set_layout;
		pipeline_layout_CI.pushConstantRangeCount = 1;
		pipeline_layout_CI.pPushConstantRanges = &push_constant_range;

		CORE_ASSERT_S(vkCreatePipelineLayout(m_device->get_device(), &pipeline_layout_CI, nullptr, &m_pipeline_layout) == VK_SUCCESS);
	}

	void render_system::create_pipeline(VkRenderPass renderPass) {

		CORE_ASSERT(m_pipeline_layout != nullptr, "", "[create_pipeline_layout()] was called bevor [m_pipeline_layout] is set");

		m_pipeline_config = pipeline_config_info();
		m_pipeline_config.pipeline_layout = m_pipeline_layout;
		m_pipeline_config.render_pass = renderPass;
		m_pipeline_config.subpass = 0;

		m_vk_pipeline = std::make_unique<vk_pipeline>(m_device, m_pipeline_config, "shaders/default.vert.spv", "shaders/default.frag.spv");
	}

}
