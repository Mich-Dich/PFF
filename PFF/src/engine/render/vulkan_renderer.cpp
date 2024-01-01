
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
#include "engine/geometry/basic_mesh.h"

#include "vulkan_renderer.h"


namespace PFF {

	// ============================================================================ setup ============================================================================

	vulkan_renderer::vulkan_renderer(std::shared_ptr<pff_window> window)
		: m_window( window ), m_active( true ) {
	
		m_device = std::make_shared<vk_device>(m_window);
		load_meshes();
		create_pipeline_layout();
		recreate_swapchian();
	}

	vulkan_renderer::~vulkan_renderer() {

		LOG(Info, "Destroying vk_pipeline");
		vkDestroyPipelineLayout(m_device->get_device(), m_pipeline_layout, nullptr);
	}

	// ============================================================================ public funcs ============================================================================

	void sierpinski(std::vector<basic_mesh::vertex>& verts, u32 depth, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3) {

		if (depth <= 0) {

			verts.push_back({ pos1 });
			verts.push_back({ pos2 });
			verts.push_back({ pos3 });
		} else {

			auto pos12 = 0.5f * (pos1 + pos2);
			auto pos23 = 0.5f * (pos2 + pos3);
			auto pos31 = 0.5f * (pos3 + pos1);
			sierpinski(verts, depth - 1, pos1, pos12, pos31);
			sierpinski(verts, depth - 1, pos12, pos2, pos23);
			sierpinski(verts, depth - 1, pos31, pos23, pos3);
		}
	}

	void vulkan_renderer::draw_frame() {

		u32 image_index;
		auto result = m_swapchain->acquireNextImage(&image_index);

		CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "", "Failed to aquire swapchain Image");

		result = m_swapchain->submitCommandBuffers(&m_command_buffers[image_index], &image_index);
		CORE_ASSERT(result == VK_SUCCESS, "", "Failed to present swapchain image");
	}

	void vulkan_renderer::wait_Idle() {

		vkDeviceWaitIdle(m_device->get_device());
	}

	void vulkan_renderer::set_size(u32 width, u32 height) {

		CORE_LOG(Info, "Resize: [" << width << ", " << height << "]");
		m_active = false;

		if (width > 0 && height > 0) {

			LOG(Info, "Resize with valid size => rebuild swapchain");
			vkDeviceWaitIdle(m_device->get_device());
			recreate_swapchian();
		}
	}

	// ============================================================================ private funcs ============================================================================


	void vulkan_renderer::recreate_swapchian() {

		m_swapchain = nullptr;
		m_swapchain = std::make_shared<vk_swapchain>(m_device, m_window->get_extend());
		create_pipeline();
		create_command_buffer();
	}

	void vulkan_renderer::recordCommandBuffer(int32 image_index) {
	}

	void vulkan_renderer::load_meshes() {

		std::vector<basic_mesh::vertex> vertices;
		vertices = {
			{{0.0f,-0.5f}, {1.0f,0.0f,0.0f}},
			{{0.5f,0.5f}, {0.0f,1.0f,0.0f}},
			{{-0.5f,0.5f}, {0.0f,0.0f,1.0f}},
		};
		//sierpinski(vertices, 3, { 0.0f, -0.5f }, { 0.5f, 0.5f }, { -0.5f, 0.5f });


		m_testmodel = std::make_unique<basic_mesh>(m_device, vertices);
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

		pipeline_config_info pipeline_config(m_swapchain->get_width(), m_swapchain->get_height(), m_pipeline_layout, m_swapchain->get_render_pass(), 0);

		m_vk_pipeline = std::make_unique<vk_pipeline>(m_device, pipeline_config, "shaders/default.vert.spv", "shaders/default.frag.spv");
	}

	void vulkan_renderer::create_command_buffer() {

		m_command_buffers.resize(m_swapchain->get_image_count());

		VkCommandBufferAllocateInfo allocat_I{};
		allocat_I.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocat_I.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocat_I.commandPool = m_device->getCommandPool();
		allocat_I.commandBufferCount = static_cast<u32>(m_command_buffers.size());

		CORE_ASSERT_S(vkAllocateCommandBuffers(m_device->get_device(), &allocat_I, m_command_buffers.data()) == VK_SUCCESS);

		for (u32 x = 0; x < m_command_buffers.size(); x++) {

			VkCommandBufferBeginInfo begin_I{};
			begin_I.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			CORE_ASSERT_S(vkBeginCommandBuffer(m_command_buffers[x], &begin_I) == VK_SUCCESS);

			VkRenderPassBeginInfo render_pass_BI{};
			render_pass_BI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_BI.renderPass = m_swapchain->get_render_pass();
			render_pass_BI.framebuffer = m_swapchain->getFrameBuffer(x);
			render_pass_BI.renderArea.offset = { 0,0 };
			render_pass_BI.renderArea.extent = m_swapchain->getSwapChainExtent();

			std::array<VkClearValue, 2> clear_values{};
			clear_values[0].color = { 0.2f, 0.2f, 0.2f, 1.0f };
			clear_values[1].depthStencil = { 1.0f,0 };
			render_pass_BI.clearValueCount = static_cast<u32>(clear_values.size());
			render_pass_BI.pClearValues = clear_values.data();

			vkCmdBeginRenderPass(m_command_buffers[x], &render_pass_BI, VK_SUBPASS_CONTENTS_INLINE);

			m_vk_pipeline->bind_commnad_buffers(m_command_buffers[x]);
			m_testmodel->bind(m_command_buffers[x]);
			m_testmodel->draw(m_command_buffers[x]);

			vkCmdEndRenderPass(m_command_buffers[x]);

			CORE_ASSERT_S(vkEndCommandBuffer(m_command_buffers[x]) == VK_SUCCESS);
		}
	}

}
