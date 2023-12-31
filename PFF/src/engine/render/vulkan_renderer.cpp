
// vulkan headers
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.h>

#include "engine/platform/pff_window.h"
#include "engine/render/vk_device.h"
#include "engine/render/vk_pipeline.h"
#include "engine/render/vk_swapchain.h"

#include "vulkan_renderer.h"


namespace PFF {

	vulkan_renderer::vulkan_renderer(std::shared_ptr<pff_window> window) 
		: m_window{ window } {
	
		m_device = std::make_shared<vk_device>(m_window);
		m_swapchain = std::make_shared<vk_swapchain>(m_device, m_window->get_extend());
		create_pipeline_layout();
		create_pipeline();
		create_command_buffer();
	}

	vulkan_renderer::~vulkan_renderer() {

		LOG(Info, "Destroying vk_pipeline");
		vkDestroyPipelineLayout(m_device->get_device(), m_pipeline_layout, nullptr);
	}

	void vulkan_renderer::create_pipeline_layout() {

		VkPipelineLayoutCreateInfo pipeline_layout_CI{};
		pipeline_layout_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_CI.setLayoutCount = 0;
		pipeline_layout_CI.pSetLayouts = nullptr;
		pipeline_layout_CI.pushConstantRangeCount = 0;
		pipeline_layout_CI.pPushConstantRanges = nullptr;

		CORE_ASSERT_S(vkCreatePipelineLayout(m_device->get_device(), &pipeline_layout_CI, nullptr, &m_pipeline_layout) == VK_SUCCESS);
	}

	void vulkan_renderer::create_pipeline() {

		pipeline_config_info pipeline_config (m_swapchain->get_width(), m_swapchain->get_height(), m_pipeline_layout, m_swapchain->get_render_pass(), 0);

		m_vk_pipeline = std::make_unique<vk_pipeline>(m_device, pipeline_config, "shaders/default.vert.spv", "shaders/default.frag.spv");
	}

	void vulkan_renderer::create_command_buffer() {


	}

	void vulkan_renderer::draw_frame() {


	}

}
