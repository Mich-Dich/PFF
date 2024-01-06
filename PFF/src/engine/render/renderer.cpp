
#include "util/pffpch.h"

#include "engine/render/render_system.h"

// DEV-ONLY
#include "engine/geometry/basic_mesh.h"
#include "engine/game_objects/game_object.h"

#include "renderer.h"

namespace PFF {


	// ==================================================================== setup ====================================================================

	renderer::renderer(std::shared_ptr<pff_window> window) 
		: m_window(window) {

		m_device = std::make_shared<vk_device>(m_window);
		recreate_swapchian();
		create_command_buffer();
		state = system_state::active;
		CORE_LOG(Trace, "renderstarted");
	}

	renderer::~renderer() {

		free_command_buffers();
	}

	// ==================================================================== public functions ====================================================================

	void renderer::add_render_system(std::unique_ptr<render_system> render_system) {

		m_render_system = std::move(render_system);
	}

	//
	void renderer::draw_frame() {
		
		if (state == system_state::active) {

			if (auto commandbuffer = begin_frame()) {

				begin_swapchain_renderpass(commandbuffer);
				m_render_system->render_game_objects(commandbuffer, m_game_objects);
				end_swapchain_renderpass(commandbuffer);

				end_frame();
			}
		}
	}

	//
	void renderer::wait_Idle() {
		
		vkDeviceWaitIdle(m_device->get_device());
	}

	//
	void renderer::set_size(u32 width, u32 height) {

		CORE_LOG(Info, "Resize: [" << width << ", " << height << "]");
		needs_to_resize = true;

		if (width > 0 && height > 0) {

			state = system_state::active;
			LOG(Info, "Resize with valid size => rebuild swapchain");
			vkDeviceWaitIdle(m_device->get_device());
			recreate_swapchian();
		} else {
			state = system_state::inactive;
		}
	}

	//
	void renderer::refresh() {

		draw_frame();
	}

	//
	void renderer::create_dummy_game_objects() {

		std::vector<basic_mesh::vertex> vertices;
		vertices = {
			{{0.0f,-0.5f}},
			{{0.5f,0.5f}},
			{{-0.5f,0.5f}},
		};
		auto model = std::make_shared<basic_mesh>(m_device, vertices);

		auto triangle = game_object::create_game_object();
		triangle.mesh = model;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform_2D.translation.x = .2f;
		triangle.transform_2D.scale = { 2.0f ,0.5f };
		triangle.transform_2D.rotation = 0.25f * two_pi<float>();

		m_game_objects.push_back(std::move(triangle));
	}

	// ==================================================================== private functions ====================================================================

	//
	VkCommandBuffer renderer::begin_frame() {

		CORE_ASSERT(!m_is_frame_started, "", "Can not begin frame if frame already in progress");

		auto result = m_swapchain->acquireNextImage(&m_current_image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {

			needs_to_resize = true;
			recreate_swapchian();
			return nullptr;			// return null to indicate start failure
		}

		CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "", "Failed to aquire swapchain Image");

		m_is_frame_started = true;
		auto command_buffer = get_current_command_buffer();

		VkCommandBufferBeginInfo command_buffer_BI{};
		command_buffer_BI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CORE_ASSERT_S(vkBeginCommandBuffer(command_buffer, &command_buffer_BI) == VK_SUCCESS);

		return command_buffer;
	}

	//
	void renderer::end_frame() {

		CORE_ASSERT(m_is_frame_started, "", "Can not end frame if not frame was started");

		auto command_buffer = get_current_command_buffer();
		CORE_ASSERT_S(vkEndCommandBuffer(command_buffer) == VK_SUCCESS);

		auto result = m_swapchain->submitCommandBuffers(&command_buffer, &m_current_image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {

			needs_to_resize = true;
			recreate_swapchian();
			// return;
		}

		CORE_ASSERT(result == VK_SUCCESS, "", "Failed to present swapchain image");
		m_is_frame_started = false;
	}

	//
	void renderer::begin_swapchain_renderpass(VkCommandBuffer commandbuffer) {

		CORE_ASSERT(m_is_frame_started, "", "Can not begin frame if frame already in progress");
		CORE_ASSERT(commandbuffer == get_current_command_buffer(), "", "Can not begin render pass on command buffer from diffrent frame");


		VkRenderPassBeginInfo render_pass_BI{};
		render_pass_BI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_BI.renderPass = m_swapchain->get_render_pass();
		render_pass_BI.framebuffer = m_swapchain->getFrameBuffer(m_current_image_index);
		render_pass_BI.renderArea.offset = { 0,0 };
		render_pass_BI.renderArea.extent = m_swapchain->getSwapChainExtent();

		std::array<VkClearValue, 2> clear_values{};
		clear_values[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clear_values[1].depthStencil = { 1.0f,0 };
		render_pass_BI.clearValueCount = static_cast<u32>(clear_values.size());
		render_pass_BI.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(commandbuffer, &render_pass_BI, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.x = 0.0f;
		viewport.width = static_cast<f32>(m_swapchain->getSwapChainExtent().width);
		viewport.height = static_cast<f32>(m_swapchain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissors{ {0,0},m_swapchain->getSwapChainExtent() };
		vkCmdSetViewport(commandbuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandbuffer, 0, 1, &scissors);
	}

	//
	void renderer::end_swapchain_renderpass(VkCommandBuffer commandbuffer) {

		CORE_ASSERT(m_is_frame_started, "", "Can not end frame if frame already in progress");
		CORE_ASSERT(commandbuffer == get_current_command_buffer(), "", "Can not end render pass on command buffer from diffrent frame");

		vkCmdEndRenderPass(commandbuffer);
	}

	//
	void renderer::create_command_buffer() {


		m_command_buffers.resize(m_swapchain->get_image_count());

		VkCommandBufferAllocateInfo allocat_I{};
		allocat_I.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocat_I.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocat_I.commandPool = m_device->get_command_pool();
		allocat_I.commandBufferCount = static_cast<u32>(m_command_buffers.size());

		CORE_ASSERT_S(vkAllocateCommandBuffers(m_device->get_device(), &allocat_I, m_command_buffers.data()) == VK_SUCCESS);
	}

	//
	void renderer::free_command_buffers() {
	}

	//
	void renderer::recreate_swapchian() {


		//m_swapchain = nullptr;

		if (m_swapchain == nullptr) {

			m_swapchain = std::make_unique<vk_swapchain>(m_device, m_window->get_extend());

		} else {

			m_swapchain = std::make_unique<vk_swapchain>(m_device, m_window->get_extend(), std::move(m_swapchain));
			if (m_swapchain->get_image_count() != m_command_buffers.size()) {

				free_command_buffers();
				create_command_buffer();
			}
		}

		// TODO: create_pipeline();
		create_command_buffer();
		needs_to_resize = false;
	}

}