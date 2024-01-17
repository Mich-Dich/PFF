
#include "util/pffpch.h"

#include "engine/render/render_system.h"
#include "engine/layer/layer.h"
#include "application.h"
#include "engine/map/game_map.h"

// DEV-ONLY
#include "engine/geometry/basic_mesh.h"
#include "engine/game_objects/game_object.h"

#include "renderer.h"

namespace PFF {

#define IMGUI_LAYER			application::get().get_imgui_layer()

	// ==================================================================== setup ====================================================================

	renderer::renderer(std::shared_ptr<pff_window> window, layer_stack* layerstack)
		: m_window(window), m_layerstack(layerstack) {

		m_device = std::make_shared<vk_device>(m_window);
		recreate_swapchian();
		create_command_buffer();
		state = system_state::active;
		CORE_LOG(Trace, "render started");

		// Create Descriptor Pool (ImGui_ImplVulkan_Init)
		// The example only requires a single combined image sampler descriptor for the font image and only uses one descriptor set (for that)
		// If you wish to load e.g. additional textures you may need to alter pools sizes.
		std::array<VkDescriptorPoolSize, 2> pool_sizes{};
		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_sizes[0].descriptorCount = m_swapchain->get_max_frames_in_flight();
		pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[1].descriptorCount = m_swapchain->get_max_frames_in_flight() * 2;

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = m_swapchain->get_max_frames_in_flight() * 2;
		pool_info.poolSizeCount = (uint32_t)ARRAY_SIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes.data();
		CORE_ASSERT(vkCreateDescriptorPool(m_device->get_device(), &pool_info, nullptr, &m_imgui_descriptor_pool) == VK_SUCCESS, "", "Failed to create descriptor Pool");
		
		createDescriptorSetLayout();

		// add first render system
		add_render_system(get_device(), get_swapchain_render_pass());

	}

	renderer::~renderer() {

		vkDestroyDescriptorPool(m_device->get_device(), m_imgui_descriptor_pool, nullptr);
		free_command_buffers();
	}

	// ==================================================================== public ====================================================================

	void renderer::add_render_system(std::shared_ptr<vk_device> device, VkRenderPass renderPass) {

		m_render_system = std::make_unique<render_system>(get_device(), get_swapchain_render_pass(), m_descriptor_set_layout);
	}

	//
	void renderer::draw_frame(f32 delta_time) {
		
		if (state == system_state::active) {

			if (auto commandbuffer = begin_frame()) {

				begin_swapchain_renderpass(commandbuffer);
				m_render_system->render_game_objects( delta_time, commandbuffer, application::get().get_current_map()->get_all_game_objects(), *application::get().get_world_layer()->get_editor_camera());
				// application::get().get_imgui_layer()->capture_current_image( ,m_swapchain->get_image_view(m_current_image_index));

				IMGUI_LAYER->begin_frame();
				for (layer* target : *m_layerstack)
					target->on_imgui_render();
				IMGUI_LAYER->end_frame(commandbuffer);

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
	void renderer::refresh(f32 delta_time) {

		draw_frame(delta_time);
	}


	// ==================================================================== private ====================================================================

	//
	void renderer::createDescriptorSetLayout() {
		
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		CORE_ASSERT(vkCreateDescriptorSetLayout(m_device->get_device(), &layoutInfo, nullptr, &m_descriptor_set_layout) == VK_SUCCESS, "", "Failes to create Descriptor_set_layout")
	}

	//
	void renderer::createDescriptorSets() {
		/*
		u32 swapchain_image_count = m_swapchain->get_image_count();

		std::vector<VkDescriptorSetLayout> layouts(swapchain_image_count, m_descriptor_set_layout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_imgui_descriptor_pool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchain_image_count);
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(swapchain_image_count);
		CORE_ASSERT(vkAllocateDescriptorSets(m_device->get_device(), &allocInfo, m_DescriptorSets.data()) == VK_SUCCESS, "", "failed to allocate descriptor sets!");

		for (size_t i = 0; i < swapchain_image_count; i++) {

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_UniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_TextureImageView;
			imageInfo.sampler = m_TextureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_DescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(m_device->get_device(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}*/
	}

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
		render_pass_BI.renderArea.extent = m_swapchain->get_swapchain_extent();

		std::array<VkClearValue, 2> clear_values{};
		clear_values[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clear_values[1].depthStencil = { 1.0f,0 };
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