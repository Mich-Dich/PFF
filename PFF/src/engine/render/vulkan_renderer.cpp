
#include "util/pffpch.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "engine/platform/pff_window.h"
#include "engine/render/vk_device.h"
#include "engine/render/vk_pipeline.h"
#include "engine/render/vk_swapchain.h"

#include "engine/geometry/basic_mesh.h"
#include "engine/game_objects/game_object.h"

#include "vulkan_renderer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"


namespace PFF {

	struct simple_push_constant_data {
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	// ============================================================================ setup ============================================================================

	vulkan_renderer::vulkan_renderer(std::shared_ptr<pff_window> window)
		: m_window( window ), m_active( true ), needs_to_resize(false) {

		m_device = std::make_shared<vk_device>(m_window);
		create_dummy_game_objects();
		create_pipeline_layout();
		recreate_swapchian();

		// Init ImGui
		// VkDescriptorPoolSize poolSizes[] = {
		// 	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
		// 	// { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		// 	// { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		// 	// { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		// 	// { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		// 	// { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		// 	// { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		// 	// { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		// 	// { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		// 	// { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		// 	// { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		// 	// { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		// };
		// VkDescriptorPoolCreateInfo poolInfo = {};
		// poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		// poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		// poolInfo.maxSets = 10 * ARRAY_SIZE(poolSizes);
		// poolInfo.poolSizeCount = (uint32_t)ARRAY_SIZE(poolSizes);
		// poolInfo.pPoolSizes = poolSizes;
		// CORE_ASSERT_S(vkCreateDescriptorPool(m_device->get_device(), &poolInfo, 0, &m_imgui_descriptor_pool) == VK_SUCCESS);
		// 
		// ImGui::CreateContext();
		// ImGui::StyleColorsDark();
		// ImGui_ImplGlfw_InitForVulkan(m_window->get_window(), false);
		// ImGui_ImplVulkan_InitInfo initInfo = {};
		// initInfo.Instance = m_device->get_instance();
		// initInfo.PhysicalDevice = m_device->get_physical_device();
		// initInfo.Device = m_device->get_device();
		// initInfo.QueueFamily = m_device->find_physical_queue_families().graphicsFamily;
		// initInfo.Queue = m_device->get_graphics_queue();
		// initInfo.DescriptorPool = m_imgui_descriptor_pool;
		// initInfo.MinImageCount = 2;
		// initInfo.ImageCount = m_swapchain->get_image_count();
		// initInfo.MSAASamples = VK_SAMPLE_COUNT_4_BIT;
		// ImGui_ImplVulkan_Init(&initInfo, m_swapchain->get_render_pass());
		// // Upload Fonts
		// 
		// // Use any command queue
		// VkCommandPool commandPool = m_device->get_command_pool();
		// VkCommandBuffer commandBuffer = commandBuffers[0];
		// 
		// CORE_ASSERT_S(vkResetCommandPool(context->device, commandPool, 0) == VK_SUCCESS);
		// VkCommandBufferBeginInfo begin_info = {};
		// begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		// CORE_ASSERT_S(vkBeginCommandBuffer(commandBuffer, &begin_info) == VK_SUCCESS);
		// 
		// ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		// 
		// CORE_ASSERT_S(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS);
		// 
		// VkSubmitInfo submitInfo = {};
		// submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		// submitInfo.commandBufferCount = 1;
		// submitInfo.pCommandBuffers = &commandBuffer;
		// CORE_ASSERT_S(vkQueueSubmit(context->graphicsQueue.queue, 1, &submitInfo, VK_NULL_HANDLE) == VK_SUCCESS);
		// 
		// CORE_ASSERT_S(vkDeviceWaitIdle(context->device) == VK_SUCCESS);
		// ImGui_ImplVulkan_DestroyFontUploadObjects();
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

		if (m_active) {

			u32 image_index;
			auto result = m_swapchain->acquireNextImage(&image_index);

			if (result == VK_ERROR_OUT_OF_DATE_KHR) {

				needs_to_resize = true;
				recreate_swapchian();
				return;
			}

			CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "", "Failed to aquire swapchain Image");

			recordCommandBuffer(image_index);
			result = m_swapchain->submitCommandBuffers(&m_command_buffers[image_index], &image_index);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {

				needs_to_resize = true;
				recreate_swapchian();
				return;
			}

			CORE_ASSERT(result == VK_SUCCESS, "", "Failed to present swapchain image");
		}
	}

	void vulkan_renderer::wait_Idle() {

		vkDeviceWaitIdle(m_device->get_device());
	}

	void vulkan_renderer::set_size(u32 width, u32 height) {

		CORE_LOG(Info, "Resize: [" << width << ", " << height << "]");
		needs_to_resize = true;

		if (width > 0 && height > 0) {

			m_active = true;
			LOG(Info, "Resize with valid size => rebuild swapchain");
			vkDeviceWaitIdle(m_device->get_device());
			recreate_swapchian();
		}
		else {
			m_active = false;
		}
	}

	void vulkan_renderer::refresh() {

		draw_frame();
	}

	// ============================================================================ private funcs ============================================================================


	void vulkan_renderer::render_game_objects(VkCommandBuffer_T* command_buffer) {

		m_vk_pipeline->bind_commnad_buffers((VkCommandBuffer)command_buffer);

		for (auto& obj : m_game_objects) {

			simple_push_constant_data PCD{};
			PCD.offset = obj.transform_2D.translation;
			PCD.color = obj.color;
			PCD.transform = obj.transform_2D.mat2();

			vkCmdPushConstants((VkCommandBuffer)command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(simple_push_constant_data), &PCD);

			obj.mesh->bind(command_buffer);
			obj.mesh->draw(command_buffer);
		}
	}

	void vulkan_renderer::recreate_swapchian() {

		//m_swapchain = nullptr;

		if (m_swapchain == nullptr) {

			m_swapchain = std::make_shared<vk_swapchain>(m_device, m_window->get_extend());
		} else {

			m_swapchain = std::make_shared<vk_swapchain>(m_device, m_window->get_extend(), std::move(m_swapchain));
			if (m_swapchain->get_image_count() != m_command_buffers.size()) {

				free_command_buffers();
				create_command_buffer();
			}
		}

		create_pipeline();
		create_command_buffer();
		needs_to_resize = false;
	}

	void vulkan_renderer::recordCommandBuffer(int32 image_index) {

		VkCommandBufferBeginInfo begin_I{};
		begin_I.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		CORE_ASSERT_S(vkBeginCommandBuffer(m_command_buffers[image_index], &begin_I) == VK_SUCCESS);

		VkRenderPassBeginInfo render_pass_BI{};
		render_pass_BI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_BI.renderPass = m_swapchain->get_render_pass();
		render_pass_BI.framebuffer = m_swapchain->getFrameBuffer(image_index);
		render_pass_BI.renderArea.offset = { 0,0 };
		render_pass_BI.renderArea.extent = m_swapchain->getSwapChainExtent();

		std::array<VkClearValue, 2> clear_values{};
		clear_values[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clear_values[1].depthStencil = { 1.0f,0 };
		render_pass_BI.clearValueCount = static_cast<u32>(clear_values.size());
		render_pass_BI.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(m_command_buffers[image_index], &render_pass_BI, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.x = 0.0f;
		viewport.width = static_cast<f32>(m_swapchain->getSwapChainExtent().width);
		viewport.height = static_cast<f32>(m_swapchain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissors{ {0,0},m_swapchain->getSwapChainExtent() };
		vkCmdSetViewport(m_command_buffers[image_index], 0, 1, &viewport);
		vkCmdSetScissor(m_command_buffers[image_index], 0, 1, &scissors);
		
		render_game_objects((VkCommandBuffer)m_command_buffers[image_index]);

		vkCmdEndRenderPass(m_command_buffers[image_index]);

		CORE_ASSERT_S(vkEndCommandBuffer(m_command_buffers[image_index]) == VK_SUCCESS);
	}

	void vulkan_renderer::create_dummy_game_objects() {

		std::vector<basic_mesh::vertex> vertices;
		vertices = {
			{{0.0f,-0.5f}, {1.0f,0.0f,0.0f}},
			{{0.5f,0.5f}, {0.0f,1.0f,0.0f}},
			{{-0.5f,0.5f}, {0.0f,0.0f,1.0f}},
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

	void vulkan_renderer::create_pipeline_layout() {

		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(simple_push_constant_data);

		VkPipelineLayoutCreateInfo pipeline_layout_CI{};
		pipeline_layout_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_CI.setLayoutCount = 0;
		pipeline_layout_CI.pSetLayouts = nullptr;
		pipeline_layout_CI.pushConstantRangeCount = 1;
		pipeline_layout_CI.pPushConstantRanges = &push_constant_range;

		CORE_ASSERT_S(vkCreatePipelineLayout(m_device->get_device(), &pipeline_layout_CI, nullptr, &m_pipeline_layout) == VK_SUCCESS);
	}

	void vulkan_renderer::create_pipeline() {

		CORE_ASSERT(m_swapchain != nullptr, "", "[create_pipeline_layout()] was called bevor [m_swapchain] is set");
		CORE_ASSERT(m_pipeline_layout != nullptr, "", "[create_pipeline_layout()] was called bevor [m_pipeline_layout] is set");

		m_pipeline_config = pipeline_config_info();
		m_pipeline_config.pipeline_layout = m_pipeline_layout;
		m_pipeline_config.render_pass = m_swapchain->get_render_pass();
		m_pipeline_config.subpass = 0;

		m_vk_pipeline = std::make_unique<vk_pipeline>(m_device, m_pipeline_config, "shaders/default.vert.spv", "shaders/default.frag.spv");
	}

	void vulkan_renderer::create_command_buffer() {

		m_command_buffers.resize(m_swapchain->get_image_count());

		VkCommandBufferAllocateInfo allocat_I{};
		allocat_I.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocat_I.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocat_I.commandPool = m_device->get_command_pool();
		allocat_I.commandBufferCount = static_cast<u32>(m_command_buffers.size());

		CORE_ASSERT_S(vkAllocateCommandBuffers(m_device->get_device(), &allocat_I, m_command_buffers.data()) == VK_SUCCESS);
	}

	void vulkan_renderer::free_command_buffers() {

		vkFreeCommandBuffers(m_device->get_device(), m_device->get_command_pool(), static_cast<u32>(m_command_buffers.size()), m_command_buffers.data());
		m_command_buffers.clear();
	}

}
