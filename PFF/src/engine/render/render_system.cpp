
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

		glm::mat4 transform{ 1.0f };
		glm::mat4 normal_matrix{ 1.0f };
	};


	render_system::render_system(std::shared_ptr<vk_device> device, VkRenderPass renderPass, VkDescriptorSetLayout descriptor_set_layout)
	 : m_device( device ) {

		PFF_PROFILE_FUNCTION();

		create_pipeline_layout(descriptor_set_layout);
		create_pipeline(renderPass);
		CORE_LOG(Trace, "render_system started");
	}

	render_system::~render_system() {

		PFF_PROFILE_FUNCTION();

		vkDestroyPipelineLayout(m_device->get_device(), m_pipeline_layout, nullptr);

		m_renderer.reset();
		m_vk_pipeline.reset();
		m_device.reset();

		LOG(Info, "shutdown");
	}

	void render_system::render_game_objects(frame_info frame_info, std::vector<game_object>& game_objects) {

		PFF_PROFILE_FUNCTION();

		m_vk_pipeline->bind_commnad_buffers(frame_info.command_buffer);

		auto projection_view = frame_info.camera->get_projection() * frame_info.camera->get_view();

		/*for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				std::cout << std::fixed << std::setprecision(2) << projection_view[i][j] << " ";
			}
			std::cout << std::endl;
		}*/

		for (auto& obj : game_objects) {
			
			simple_push_constant_data PCD{};
			auto modle_matrix = obj.transform.mat4();
			PCD.transform = projection_view * modle_matrix;
			PCD.normal_matrix = obj.transform.normal_matrix();

			vkCmdPushConstants(frame_info.command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(simple_push_constant_data), &PCD);

			obj.mesh->bind(frame_info.command_buffer);
			obj.mesh->draw(frame_info.command_buffer);
		}
	}

	void render_system::create_pipeline_layout(VkDescriptorSetLayout descriptor_set_layout) {

		PFF_PROFILE_FUNCTION();

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

		PFF_PROFILE_FUNCTION();

		CORE_ASSERT(m_pipeline_layout != nullptr, "", "[create_pipeline_layout()] was called bevor [m_pipeline_layout] is set");

		m_pipeline_config = pipeline_config_info();
		m_pipeline_config.pipeline_layout = m_pipeline_layout;
		m_pipeline_config.render_pass = renderPass;
		m_pipeline_config.subpass = 0;

		m_vk_pipeline = std::make_unique<vk_pipeline>(m_device, m_pipeline_config, "shaders/default.vert.spv", "shaders/default.frag.spv");
	}

}
