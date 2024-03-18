
#include "util/pffpch.h"

#include "engine/render/render_system/mesh_render_system.h"
#include "engine/render/vk_buffer.h"
#include "engine/platform/pff_window.h"
#include "engine/layer/layer.h"
#include "application.h"
#include "engine/map/game_map.h"

// DEV-ONLY
#include "engine/geometry/basic_mesh.h"
#include "engine/game_objects/game_object.h"

#include "renderer.h"

namespace PFF {

#define IMGUI_LAYER			application::get().get_imgui_layer()

	struct global_ubo {
		glm::mat4 projectionView{ 1.f };
		glm::vec3 light_direction = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};

	// ==================================================================== setup ====================================================================

	renderer::renderer(ref<pff_window> window, layer_stack* layerstack)
		: m_window(window), m_layerstack(layerstack) {

		PFF_PROFILE_FUNCTION();

		m_device = std::make_shared<vk_device>(m_window);
		recreate_swapchian();
		create_command_buffer();
		m_state = system_state::suspended;
		CORE_LOG(Trace, "render started");

		// make global UBO
		m_global_UBO_buffer = std::vector<scope_ref<vk_buffer>>(vk_swapchain::MAX_FRAMES_IN_FLIGHT);
		for (u16 x = 0; x < m_global_UBO_buffer.size(); x++) {

			m_global_UBO_buffer[x] = std::make_unique<vk_buffer>( m_device, sizeof(global_ubo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			m_global_UBO_buffer[x]->map();
		}

		// make global descriptor pool
		CORE_LOG(Trace, "build descriptor pool");
		m_global_descriptor_pool = vk_descriptor_pool::builder(m_device)
			.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)

			.setMaxSets(1000)
			.setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
			.build();

		CORE_LOG(Trace, "build descriptor_set layout");
		m_global_set_layout = vk_descriptor_set_layout::builder(m_device)
			.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)					// binding for renderer & ImGui
			.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)		// for ImGui
			.build();

		CORE_LOG(Trace, "Resize swapchain");
		m_global_descriptor_set.resize(vk_swapchain::MAX_FRAMES_IN_FLIGHT);
		for (int16 x = 0; x < m_global_descriptor_set.size(); x++) {

			auto buffer_info = m_global_UBO_buffer[x]->descriptor_info();
			vk_descriptor_writer(*m_global_set_layout, *m_global_descriptor_pool)
				.write_buffer(0, &buffer_info)
				.build(m_global_descriptor_set[x]);
		}

		// render system for: game objects
		CORE_LOG(Trace, "add mesh render system");
		m_mesh_render_system = create_scoped_ref<mesh_render_system>(get_device(), get_swapchain_render_pass(), m_global_set_layout->get_descriptor_set_layout());

		// render system for: ImGui
		//CORE_LOG(Trace, "add ImGui render system");
		//m_ui_render_system = create_scoped_ref<ui_render_system>(get_device(), get_swapchain_render_pass(), m_global_set_layout->get_descriptor_set_layout());
		//application::get().get_imgui_layer()->
	}

	renderer::~renderer() {

		PFF_PROFILE_FUNCTION();

		m_state = system_state::inactive;

		for (u16 x = 0; x < m_global_UBO_buffer.size(); x++)
			m_global_UBO_buffer[x].reset();

		free_command_buffers();

		m_ui_render_system.reset();
		m_mesh_render_system.reset();
		m_global_set_layout.reset();
		m_global_descriptor_pool.reset();
		m_global_UBO_buffer.clear();
		m_window.reset();
		m_swapchain.reset();
		m_device.reset();


		LOG(Info, "shutdown");
	}

	// ==================================================================== public ====================================================================

	void renderer::add_render_system(VkDescriptorSetLayout descriptor_set_layout) {

		PFF_PROFILE_FUNCTION();

		CORE_LOG(Error, "Not implemented yet!");
		//m_mesh_render_system = std::make_unique<render_system>(get_device(), get_swapchain_render_pass(), descriptor_set_layout);
	}

	//
	void renderer::draw_frame(const f32 delta_time) {

		PFF_PROFILE_FUNCTION();

		if (m_state != system_state::active)
			return;

		if (auto commandbuffer = begin_frame()) {

			frame_info frame_info{ m_current_image_index, delta_time, commandbuffer, m_world_Layer->get_editor_camera(), m_global_descriptor_set[m_current_image_index] };

			// update
			global_ubo ubo{};
			ubo.projectionView = frame_info.camera->get_projection() * frame_info.camera->get_view();
			m_global_UBO_buffer[m_current_image_index]->write_to_buffer(&ubo);
			m_global_UBO_buffer[m_current_image_index]->flush();

			// render
			begin_swapchain_renderpass(commandbuffer);
			m_mesh_render_system->render_game_objects(frame_info, m_world_Layer->get_current_map()->get_all_game_objects());
			// application::get().get_imgui_layer()->capture_current_image( ,m_swapchain->get_image_view(m_current_image_index));

			//m_ui_render_system->bind_pipeline(frame_info);
			IMGUI_LAYER->begin_frame();
			for (layer* target : *m_layerstack)
				target->on_imgui_render();
			IMGUI_LAYER->end_frame(commandbuffer);

			end_swapchain_renderpass(commandbuffer);
			end_frame();
		}
	}

	//
	void renderer::wait_Idle() {

		PFF_PROFILE_FUNCTION();

		vkDeviceWaitIdle(m_device->get_device());
	}

	//
	void renderer::set_size(const u32 width, const u32 height) {

		PFF_PROFILE_FUNCTION();

		CORE_LOG(Trace, "Resize: [" << width << ", " << height << "]");
		needs_to_resize = true;

		if (width > 0 && height > 0) {

			m_state = system_state::active;
			vkDeviceWaitIdle(m_device->get_device());
			recreate_swapchian();
		} else {
			m_state = system_state::inactive;
		}
	}

	//
	void renderer::refresh(const f32 delta_time) {

		draw_frame(delta_time);
	}


	// ==================================================================== private ====================================================================

	//
	VkCommandBuffer renderer::begin_frame() {

		PFF_PROFILE_FUNCTION();

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

		PFF_PROFILE_FUNCTION();

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

		PFF_PROFILE_FUNCTION();

		CORE_ASSERT(m_is_frame_started, "", "Can not begin frame if frame already in progress");
		CORE_ASSERT(commandbuffer == get_current_command_buffer(), "", "Can not begin render pass on command buffer from diffrent frame");

		std::array<VkClearValue, 2> clear_values{};
		clear_values[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clear_values[1].depthStencil = { 1.0f,0 };

		VkRenderPassBeginInfo render_pass_BI{};
		render_pass_BI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_BI.renderPass = m_swapchain->get_render_pass();
		render_pass_BI.framebuffer = m_swapchain->getFrameBuffer(m_current_image_index);
		render_pass_BI.renderArea.offset = { 0,0 };
		render_pass_BI.renderArea.extent = m_swapchain->get_swapchain_extent();
		render_pass_BI.clearValueCount = static_cast<u32>(clear_values.size());
		render_pass_BI.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(commandbuffer, &render_pass_BI, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.x = 0.0f;
		viewport.width = static_cast<f32>(m_swapchain->get_swapchain_extent().width);
		viewport.height = static_cast<f32>(m_swapchain->get_swapchain_extent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissors{ {0,0},m_swapchain->get_swapchain_extent() };
		vkCmdSetViewport(commandbuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandbuffer, 0, 1, &scissors);
	}

	//
	void renderer::end_swapchain_renderpass(VkCommandBuffer commandbuffer) {

		PFF_PROFILE_FUNCTION();

		CORE_ASSERT(m_is_frame_started, "", "Can not end frame if frame already in progress");
		CORE_ASSERT(commandbuffer == get_current_command_buffer(), "", "Can not end render pass on command buffer from diffrent frame");

		vkCmdEndRenderPass(commandbuffer);
	}

	//
	void renderer::create_command_buffer() {

		PFF_PROFILE_FUNCTION();

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

		vkFreeCommandBuffers(m_device->get_device(), m_device->get_command_pool(), static_cast<int>(m_command_buffers.size()), m_command_buffers.data());
	}

	//
	void renderer::recreate_swapchian() {

		PFF_PROFILE_FUNCTION();

		// CORE_LOG(Warn, "size: " << m_window->get_extend().width << " / " << m_window->get_extend().height);

		if (m_swapchain == nullptr) {

			m_swapchain = std::make_shared<vk_swapchain>(m_device, m_window->get_extend());
			create_command_buffer();

		} else {

			m_swapchain = std::make_shared<vk_swapchain>(m_device, m_window->get_extend(), std::move(m_swapchain));
			if (m_swapchain->get_image_count() != m_command_buffers.size()) {

				free_command_buffers();
				create_command_buffer();
			}
		}

		// TODO: create_pipeline();
		needs_to_resize = false;
	}

}
