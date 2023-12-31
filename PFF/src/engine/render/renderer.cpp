
#include "util/pffpch.h"

#include "engine/render/render_system.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imconfig.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "imgui_tables.cpp"
#include "imgui_internal.h"
#include "imgui.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "imgui_demo.cpp"
#include "imgui_impl_glfw.cpp"
//#include "imgui_impl_vulkan_but_better.h"

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
		
		// add first render system
		add_render_system(get_device(), get_swapchain_render_pass());

		imgui_init();
	}

	renderer::~renderer() {

		free_command_buffers();
		vkDestroyDescriptorPool(m_device->get_device(), m_imgui_descriptor_pool, nullptr);
		imgui_sutdown();
	}

	// ==================================================================== public functions ====================================================================

	void renderer::add_render_system(std::shared_ptr<vk_device> device, VkRenderPass renderPass) {

		m_render_system = std::make_unique<render_system>(get_device(), get_swapchain_render_pass());
	}

	//
	void renderer::draw_frame() {
		
		if (state == system_state::active) {

			if (auto commandbuffer = begin_frame()) {

				begin_swapchain_renderpass(commandbuffer);
				m_render_system->render_game_objects(commandbuffer, m_game_objects);

				ImGui_ImplVulkan_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				ImGui::ShowDemoWindow();

				ImGui::Render();
				ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandbuffer, 0);

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

	void renderer::imgui_init() {

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_Docking_enabled;
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(m_window->get_window(), true);
		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.Instance = m_device->get_instance();
		init_info.PhysicalDevice = m_device->get_physical_device();
		init_info.Device = m_device->get_device();
		init_info.QueueFamily = m_device->find_physical_queue_families().graphicsFamily;
		init_info.Queue = m_device->get_graphics_queue();
		init_info.DescriptorPool = m_imgui_descriptor_pool;
		init_info.Subpass = m_render_system->get_pipeline_subpass();
		init_info.MinImageCount = m_device->get_swap_chain_support().capabilities.minImageCount;
		init_info.ImageCount = m_swapchain->get_image_count();
		//init_info.PipelineCache = g_PipelineCache;
		//init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		//init_info.Allocator = nullptr;
		//init_info.CheckVkResultFn = check_vk_result;
		CORE_ASSERT(ImGui_ImplVulkan_Init(&init_info, m_swapchain->get_render_pass()), "", "");

		// Use any command queue
		VkCommandBuffer command_buffer = m_device->begin_single_time_commands();
		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
		m_device->end_single_time_commands(command_buffer);

		CORE_ASSERT(vkDeviceWaitIdle(m_device->get_device()) == VK_SUCCESS, "", "Failed wait idle");
		ImGui_ImplVulkan_DestroyFontUploadObjects();

	}

	void renderer::imgui_sutdown() {

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
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