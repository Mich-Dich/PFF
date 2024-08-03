
#include "util/pffpch.h"

// ========== vulkan utils ============
#include "vk_types.h"
#define VMA_IMPLEMENTATION
#include "vendor/vk_mem_alloc.h"
#include "vendor/VkBootstrap.h"
#include "vk_initializers.h"
#include "vk_instance.h"
#include "vk_image.h"
#include "vk_pipeline.h"

// ========== ImGui ============
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "engine/layer/imgui_layer.h"

// ========== misc ============
#include "engine/io_handler/file_loader.h"
#include "engine/platform/pff_window.h"
#include "GLFW/glfw3.h"
#include "application.h"
#include "util/io/serializer.h"
#include "util/UI/pannel_collection.h"
#include "engine/layer/layer_stack.h"
#include "engine/layer/layer.h"
#include "engine/world/map.h"

#include <cstdlib> // for system calls (conpieling shaders)

#include "util/ui/pannel_collection.h"

#include "vk_renderer.h"

namespace PFF::render::vulkan {

	vk_renderer vk_renderer::s_instance;

	static std::vector<std::vector<std::function<void()>>> s_resource_free_queue;






	void deletion_queue:: setup(VkDevice device, VmaAllocator allocator) {
		
		m_dq_device = device; 
		m_dq_allocator = allocator;
	}

	void deletion_queue::cleanup() {
		
		m_dq_device = nullptr;
		m_dq_allocator = nullptr;
		
		CORE_LOG_SHUTDOWN();
	}

	void deletion_queue::push_func(std::function<void()>&& function) { m_deletors.push_back(function); }

	void deletion_queue::flush() {

#define IS_OF_TYPE(name)							entry.first == std::type_index(typeid(name))
#define USE_AS(name)								static_cast<name>(entry.second)
#define VK_DESTROY_FUNC(name)						if (IS_OF_TYPE(Vk##name))	{vkDestroy##name(m_dq_device, USE_AS(Vk##name), nullptr); }

		for (auto it = m_deletors.rbegin(); it != m_deletors.rend(); it++)
			(*it)();
		m_deletors.clear();
				
		for (const auto& entry : m_pointers) {

			VK_DESTROY_FUNC(Sampler)
			else VK_DESTROY_FUNC(CommandPool)
			else VK_DESTROY_FUNC(DescriptorPool)
			else VK_DESTROY_FUNC(Fence)
			else VK_DESTROY_FUNC(DescriptorSetLayout)
			else VK_DESTROY_FUNC(Pipeline)
			else VK_DESTROY_FUNC(PipelineLayout)

			else if (IS_OF_TYPE(ref<image>*))						{ USE_AS(ref<image>*)->reset(); }
			else if (IS_OF_TYPE(vk_buffer*))						{ GET_RENDERER.destroy_buffer(*USE_AS(vk_buffer*)); }
			else if (IS_OF_TYPE(descriptor_allocator_growable*))	{ USE_AS(descriptor_allocator_growable*)->destroy_pools(m_dq_device); }

			else if (IS_OF_TYPE(descriptor_allocator*)) {

				USE_AS(descriptor_allocator*)->clear_descriptors(m_dq_device);
				USE_AS(descriptor_allocator*)->destroy_pool(m_dq_device);
			}

			else if (IS_OF_TYPE(image*)) {

				image* loc_image = USE_AS(image*);
				vkDestroyImageView(m_dq_device, loc_image->get_image_view(), nullptr);
				vmaDestroyImage(m_dq_allocator, loc_image->get_image(), loc_image->get_allocation());
			}

			else
				CORE_LOG(Error, "Renderer deletion queue used with an unknown type [" << entry.first.name() << "]");
		}
		m_pointers.clear();
	}

	// ============================================= setup  ============================================= 

	void vk_renderer::setup(ref<pff_window> window, ref<PFF::layer_stack> layer_stack) {

		m_window = window;
		m_layer_stack = layer_stack;

		CORE_LOG_INIT();

		PFF::render::util::compile_shaders_in_dir("../PFF/shaders", true);

		//make the vulkan instance, with basic debug features
		vkb::InstanceBuilder builder;
		auto inst_ret = builder.set_app_name("PFF_GameEngine")
			.request_validation_layers(true)
			.use_default_debug_messenger()
			.require_api_version(1, 3, 0)
			.build();

		vkb::Instance vkb_inst = inst_ret.value();

		//grab the instance 
		m_instance = vkb_inst.instance;
		m_debug_messenger = vkb_inst.debug_messenger;

		m_window->create_vulkan_surface(m_instance, &m_surface);

		//vulkan 1.3 features
		VkPhysicalDeviceVulkan13Features features{};
		features.dynamicRendering = true;
		features.synchronization2 = true;

		//vulkan 1.2 features
		VkPhysicalDeviceVulkan12Features features12{};
		features12.bufferDeviceAddress = true;
		features12.descriptorIndexing = true;

		// select a gpu that can write to GLFW-surface and supports vulkan 1.3 with the correct features
		vkb::PhysicalDeviceSelector selector{ vkb_inst };
		vkb::PhysicalDevice physicalDevice = selector
			.set_minimum_version(1, 3)
			.set_required_features_13(features)
			.set_required_features_12(features12)
			.set_surface(m_surface)
			.select()
			.value();

		//create the final vulkan device
		vkb::DeviceBuilder deviceBuilder{ physicalDevice };
		vkb::Device vkbDevice = deviceBuilder.build().value();

		// Get the VkDevice handle used in the rest of a vulkan application
		m_device = vkbDevice.device;
		m_chosenGPU = physicalDevice.physical_device;

		// use vkbootstrap to get a Graphics queue
		m_graphics_queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		m_graphics_queue_family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

		// initialize the memory allocator
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_chosenGPU;
		allocatorInfo.device = m_device;
		allocatorInfo.instance = m_instance;
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaCreateAllocator(&allocatorInfo, &m_allocator);

		s_resource_free_queue.resize(FRAME_COUNT);

		// Setup deletion queues
		m_deletion_queue.setup(m_device, m_allocator);
		for (int i = 0; i < FRAME_COUNT; i++)
			m_frames[i].deletion_queue.setup(m_device, m_allocator);



		init_commands();
		init_swapchain();
		init_sync_structures();
		init_descriptors();
		init_pipelines();
		init_default_data();

		m_is_initialized = true;
	}

	void vk_renderer::shutdown() {
		
		if (!m_is_initialized)
			return;

		vkDeviceWaitIdle(m_device);
		m_state = system_state::inactive;
		serialize(PFF::serializer::option::save_to_file);

		for (auto frame : m_frames) {

			vkDestroyCommandPool(m_device, frame.command_pool, nullptr);
			vkDestroyFence(m_device, frame.render_fence, nullptr);
			vkDestroySemaphore(m_device, frame.render_semaphore, nullptr);
			vkDestroySemaphore(m_device, frame.swapchain_semaphore, nullptr);

			frame.frame_descriptors.clear_pools(m_device);
			frame.deletion_queue.flush();
		}

		m_deletion_queue.flush();
		m_deletion_queue.cleanup();
		
		destroy_swapchain();
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vmaDestroyAllocator(m_allocator);
		vkDestroyDevice(m_device, nullptr);
		vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
		vkDestroyInstance(m_instance, nullptr);
		
		m_window.reset();
		m_layer_stack.reset();

		CORE_LOG_SHUTDOWN();
	}

	// =======================================================================================================================================================================================
	// PUBLIC FUNCTIONS
	// =======================================================================================================================================================================================

	void vk_renderer::imgui_init() {
		
		if (m_imgui_initalized)
			return;

		// create descriptor pool for IMGUI (the size of the pool is very oversize, but it's copied from imgui demo itself)
		VkDescriptorPoolSize pool_sizes[] = { 
			{ VK_DESCRIPTOR_TYPE_SAMPLER,					1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,		1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,		1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,	1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,	1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,			1000 },
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = static_cast<u32>(std::size(pool_sizes));
		pool_info.pPoolSizes = pool_sizes;
		VK_CHECK_S(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_imgui_desc_pool));

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
		// Viewport enable flags (require both ImGuiBackendFlags_PlatformHasViewports + ImGuiBackendFlags_RendererHasViewports set by the respective backends)
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows	
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		io.IniFilename = UI::ini_file_location.c_str();

		// check for imgui ini-file or copy from default file
		if (!std::filesystem::exists(UI::ini_file_location)) {

			std::ofstream file(UI::ini_file_location);
			CORE_ASSERT(file.is_open(), "", "Failed to open [" << UI::ini_file_location << "] default: [" << io.IniFilename << "]");

			std::ifstream default_config_file("./defaults/imgui_config.ini");
			CORE_ASSERT(default_config_file.is_open(), "", "Failed to open [default_config_file]");

			file << default_config_file.rdbuf();

			default_config_file.close();
			file.close();
		}

		CORE_ASSERT(ImGui_ImplGlfw_InitForVulkan(m_window->get_window(), true), "", "Failed to initalize imgui -> init GLFW for Vulkan");

		// this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = m_instance;
		init_info.PhysicalDevice = m_chosenGPU;
		init_info.Device = m_device;
		init_info.Queue = m_graphics_queue;
		init_info.DescriptorPool = m_imgui_desc_pool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.UseDynamicRendering = true;
		init_info.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
		init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &m_swapchain_image_format;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		CORE_ASSERT(ImGui_ImplVulkan_Init(&init_info), "", "Failed to initalize ImGui-Vulkan");

		// execute a gpu command to upload imgui font textures
		// immediate_submit([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); });

		// serializer::yaml(config::get_filepath_from_configtype(config::file::editor), "UI", serializer::option::load_from_file)
		// 	.entry(KEY_VALUE(UI::THEME::main_color))
		// 	.entry(KEY_VALUE(UI::THEME::UI_theme));
		// UI::THEME::update_UI_theme();


		//void vk_renderer::createTextureSampler() {  ===============================================================
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VK_CHECK_S(vkCreateSampler(m_device, &samplerInfo, nullptr, &m_texture_sampler));
		// }  ===========================================================================================

		m_draw_image.generate_descriptor_set(m_texture_sampler, VK_IMAGE_LAYOUT_GENERAL);

		m_deletion_queue.push_pointer(m_texture_sampler);

		m_imgui_initalized = true;
	}


	void vk_renderer::imgui_create_fonts() { immediate_submit([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); }); }

	void vk_renderer::imgui_destroy_fonts() { immediate_submit([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_DestroyFontsTexture(); }); }


	void vk_renderer::imgui_shutdown() { 

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		vkDestroyDescriptorPool(m_device, m_imgui_desc_pool, nullptr);

		m_imgui_initalized = false;
	}

	
	void vk_renderer::draw_frame(f32 delta_time) {

		if (m_state != system_state::active)
			return;
		
		//wait until the gpu has finished rendering the last frame. Timeout of 1 second
		VK_CHECK_S(vkWaitForFences(m_device, 1, &get_current_frame().render_fence, true, 1000000000));

		get_current_frame().deletion_queue.flush();
		get_current_frame().frame_descriptors.clear_pools(m_device);
		{	// Free resources in queue
			for (auto& func : s_resource_free_queue[m_frame_number % FRAME_COUNT]) {
				CORE_LOG(Info, "Executing free QUEUE")
				func();
			}

			s_resource_free_queue[m_frame_number % FRAME_COUNT].clear();
		}

		uint32_t swapchain_image_index;
		VkResult e = vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, get_current_frame().swapchain_semaphore, nullptr, &swapchain_image_index);
		if (e == VK_ERROR_OUT_OF_DATE_KHR) {
			m_resize_nedded = true;
			resize_swapchain();
			return;					// this skips a frame, but it fine
		}

		VK_CHECK_S(vkResetFences(m_device, 1, &get_current_frame().render_fence));

		VkCommandBuffer cmd = get_current_frame().main_command_buffer;
		VK_CHECK_S(vkResetCommandBuffer(cmd, 0));

		VkCommandBufferBeginInfo cmdBeginInfo = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		if (m_imgui_initalized && !m_render_swapchain) {

			m_draw_extent.width = std::min(m_draw_image.get_width(), m_imugi_viewport_size.x) * (u32)m_render_scale;
			m_draw_extent.height = std::min(m_draw_image.get_height(), m_imugi_viewport_size.y) * (u32)m_render_scale;
		} else {

			m_draw_extent.width = std::min(m_swapchain_extent.width, m_draw_image.get_width()) * (u32)m_render_scale;
			m_draw_extent.height = std::min(m_swapchain_extent.height, m_draw_image.get_height()) * (u32)m_render_scale;
		}

		VK_CHECK_S(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		util::transition_image(cmd, m_draw_image.get_image(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		draw_internal(cmd);

		util::transition_image(cmd, m_draw_image.get_image(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		util::transition_image(cmd, m_depth_image.get_image(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

		draw_geometry(cmd);

		//draw_debug(cmd);

		if (m_render_swapchain) {

			//transition the draw image and the swapchain image into their correct transfer layouts
			util::transition_image(cmd, m_draw_image.get_image(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			util::copy_image_to_image(cmd, m_draw_image.get_image(), m_swapchain_images[swapchain_image_index], m_draw_extent, m_swapchain_extent);						// copy from draw_image into swapchain

		} else 
			util::transition_image(cmd, m_draw_image.get_image(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);


		// =========================================================== draw imgui ===========================================================
		if (m_imgui_initalized) {
		
			ImGui::SetCurrentContext(application::get().get_imgui_layer()->get_context());

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			for (layer* layer : *m_layer_stack) 
				layer->on_imgui_render();

			ImGui::EndFrame();
			ImGui::Render();

			// update other platform windows
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);

			if (m_render_swapchain) {
				
				util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
				draw_imgui(cmd, m_swapchain_image_views[swapchain_image_index]);		//draw imgui into the swapchain image
				util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

			} else {

				util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
				draw_imgui(cmd, m_swapchain_image_views[swapchain_image_index]);		//draw imgui into the swapchain image
				util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
			}
		}
		// =========================================================== draw imgui ===========================================================

		else
			util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
				
		VK_CHECK_S(vkEndCommandBuffer(cmd));

		VkCommandBufferSubmitInfo cmdinfo = init::command_buffer_submit_info(cmd);
		VkSemaphoreSubmitInfo waitInfo = init::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, get_current_frame().swapchain_semaphore);
		VkSemaphoreSubmitInfo signalInfo = init::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame().render_semaphore);
		VkSubmitInfo2 submit = init::submit_info(&cmdinfo, &signalInfo, &waitInfo);
		VK_CHECK_S(vkQueueSubmit2(m_graphics_queue, 1, &submit, get_current_frame().render_fence));		// [m_render_fence] will now block until the graphic commands finish execution

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_swapchain;
		presentInfo.swapchainCount = 1;
		presentInfo.pWaitSemaphores = &get_current_frame().render_semaphore;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pImageIndices = &swapchain_image_index;
		VkResult queue_present_result = vkQueuePresentKHR(m_graphics_queue, &presentInfo);
		if (queue_present_result == VK_ERROR_OUT_OF_DATE_KHR) {
			m_resize_nedded = true;
			resize_swapchain();
		}

		if (m_frame_number > 184467440737095)
			m_frame_number = m_frame_number % FRAME_COUNT;

		m_frame_number++;
	}

	void vk_renderer::refresh(f32 delta_time) {
	
		//CORE_LOG(Debug, "Refreching renderer");
		resize_swapchain();
		draw_frame(delta_time);
	}

	void vk_renderer::set_size(u32 width, u32 height) {}

	void vk_renderer::wait_idle() { vkDeviceWaitIdle(m_device); }

	void vk_renderer::immediate_submit(std::function<void()>&& function) {

		VK_CHECK_S(vkResetFences(m_device, 1, &m_immFence));
		VK_CHECK_S(vkResetCommandBuffer(m_immCommandBuffer, 0));

		VkCommandBuffer cmd = m_immCommandBuffer;
		VkCommandBufferBeginInfo cmdBeginInfo = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		VK_CHECK_S(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		function();

		VK_CHECK_S(vkEndCommandBuffer(cmd));
		VkCommandBufferSubmitInfo cmdinfo = init::command_buffer_submit_info(cmd);
		VkSubmitInfo2 submit = init::submit_info(&cmdinfo, nullptr, nullptr);

		// submit command buffer to the queue and execute it.
		//  _renderFence will now block until the graphic commands finish execution
		VK_CHECK_S(vkQueueSubmit2(m_graphics_queue, 1, &submit, m_immFence));
		VK_CHECK_S(vkWaitForFences(m_device, 1, &m_immFence, true, 9999999999));
	}

	void vk_renderer::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function) {

		VK_CHECK_S(vkResetFences(m_device, 1, &m_immFence));
		VK_CHECK_S(vkResetCommandBuffer(m_immCommandBuffer, 0));

		VkCommandBuffer cmd = m_immCommandBuffer;
		VkCommandBufferBeginInfo cmdBeginInfo = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		VK_CHECK_S(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		function(cmd);

		VK_CHECK_S(vkEndCommandBuffer(cmd));
		VkCommandBufferSubmitInfo cmdinfo = init::command_buffer_submit_info(cmd);
		VkSubmitInfo2 submit = init::submit_info(&cmdinfo, nullptr, nullptr);

		// submit command buffer to the queue and execute it.
		//  _renderFence will now block until the graphic commands finish execution
		VK_CHECK_S(vkQueueSubmit2(m_graphics_queue, 1, &submit, m_immFence));
		VK_CHECK_S(vkWaitForFences(m_device, 1, &m_immFence, true, 9999999999));
	}

	void vk_renderer::submit_resource_free(std::function<void()>&& func) {

		if (m_state == system_state::active)
			s_resource_free_queue[m_frame_number % FRAME_COUNT].emplace_back(func);
		else
			func();		// If renderer suspended/inactive, resource_free should always be possible
	}

	// =======================================================================================================================================================================================
	// PRIVATE FUNCTIONS
	// =======================================================================================================================================================================================

	// ================================================================================ INIT FUNCTION ================================================================================

	void vk_renderer::init_default_data() {

#define MESH_SOURCE 1

#if MESH_SOURCE == 0
		
		T_test_meshes = IO::mesh_loader::load_gltf_meshes("../PFF/assets/meshes/basicmesh.glb").value();

#elif MESH_SOURCE == 1
		
		T_test_meshes = IO::mesh_loader::load_gltf_meshes("../PFF/assets/meshes/BP-688.glb").value();

#elif MESH_SOURCE == 2 	// procedural terrain test

//#define PROFILE_GENERATION

		geometry::mesh loc_mesh{};
		loc_mesh.name = "procedural_test";

#ifdef PROFILE_GENERATION
		for (size_t i = 0; i < 10; i++) {
			loc_mesh = geometry::mesh{};		// reset
			CORE_LOG(Trace, "starting mesh generation iteration: " << i);
			PFF_PROFILE_SCOPE("Procedural Grid Mesh");
#endif
		
		PFF::util::noise noise(PFF::util::noise_type::perlin);
		noise.Set_fractal_type(PFF::util::fractal_type::FBm);
		noise.set_frequency(0.005f);
		noise.set_fractal_octaves(4);
		noise.set_fractal_lacunarity(2.3f);

		const glm::ivec2 grid_size = glm::ivec2(5);			// number of grid tiles
		const glm::vec2 grid_tile_size = glm::ivec2(1000);	// size of a grid tile
		const glm::vec2 grid_resolution = glm::ivec2(100);
		const int iterations_x = static_cast<u32>(grid_size.x * grid_resolution.x);
		const int iterations_y = static_cast<u32>(grid_size.y * grid_resolution.y);

		const glm::vec2 offset = glm::vec2(
			(static_cast<f32>(grid_size.x) / 2) * grid_tile_size.x,
			(static_cast<f32>(grid_size.y) / 2) * grid_tile_size.y
		);
		const glm::vec2 pos_multiplier = glm::vec2(
			(grid_tile_size.x / grid_resolution.x),
			(grid_tile_size.y / grid_resolution.y)
		);
		
		{
#ifdef PROFILE_GENERATION
			PFF_PROFILE_SCOPE("Procedural Grid Mesh - resize vectors");
#endif
			loc_mesh.m_vertices.resize((iterations_x + 1) * (iterations_y + 1));
			loc_mesh.m_indices.resize((iterations_x * iterations_y) * 6);
		}

		{
#ifdef PROFILE_GENERATION
			PFF_PROFILE_SCOPE("Procedural Grid Mesh - generate vertexes");
#endif
			int counter_vert = 0;
			for (int y = 0; y <= iterations_y; ++y) {
				for (int x = 0; x <= iterations_x; ++x) {

					loc_mesh.m_vertices[counter_vert] = geometry::vertex(
						glm::vec3(
							(x * pos_multiplier.x) - offset.x,
							noise.get_noise((f32)x, (f32)y) * 500.f,
							(y * pos_multiplier.y) - offset.y
						),
						{ 0, 0, 1 },
						glm::vec4{ 1.f },
						static_cast<f32>(x) / grid_resolution.x,
						static_cast<f32>(y) / grid_resolution.y
					);
					counter_vert++;
				}
			}
		}
			
		{
#ifdef PROFILE_GENERATION
			PFF_PROFILE_SCOPE("Procedural Grid Mesh - generate triange");
#endif
			int counter = 0;
			for (int y = 0; y < iterations_y; y++) {
				for (int x = 0; x < iterations_x; x++) {

					int top_left = y * ((iterations_x)+1) + x;
					int top_right = top_left + 1;
					int bottom_left = (y + 1) * ((iterations_x)+1) + x;
					int bottom_right = bottom_left + 1;

					loc_mesh.m_indices[counter + 0] = top_left;
					loc_mesh.m_indices[counter + 1] = bottom_left;
					loc_mesh.m_indices[counter + 2] = top_right;

					loc_mesh.m_indices[counter + 3] = top_right;
					loc_mesh.m_indices[counter + 4] = bottom_left;
					loc_mesh.m_indices[counter + 5] = bottom_right;
					counter += 6;
				}
			}
		}

		geometry::Geo_surface new_surface{};
		new_surface.count = static_cast<int>(loc_mesh.m_indices.size());
		loc_mesh.surfaces.push_back(new_surface);

#ifdef PROFILE_GENERATION
		}
#endif
		
		T_test_meshes.emplace_back(create_ref<geometry::mesh>(std::move(loc_mesh)));
		
#endif

		if (T_test_meshes.size() > 0) {

			for (auto mesh : T_test_meshes) 
				mesh->mesh_buffers = upload_mesh(mesh->m_indices, mesh->m_vertices);
		}

		serialize(PFF::serializer::option::load_from_file);

		//3 default textures, white, grey, black. 1 pixel each
		u32 white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
		m_white_image = create_ref<image>((void*)&white, 1, 1, image_format::RGBA);

		u32 grey = glm::packUnorm4x8(glm::vec4(0.4f, 0.44f, 0.4f, 1));
		m_black_image = create_ref<image>((void*)&grey, 1, 1, image_format::RGBA);

		u32 black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));	// TODO: chnage A to 1
		m_grey_image = create_ref<image>((void*)&black, 1, 1, image_format::RGBA);

		//checkerboard image
		const int EDGE_LENGTH = 2;
		u32 color = glm::packUnorm4x8(glm::vec4(0.2f, 0.2f, 0.2f, 1));
		std::array<u32, EDGE_LENGTH * EDGE_LENGTH > pixels; //for checkerboard texture
		for (int x = 0; x < EDGE_LENGTH; x++)
			for (int y = 0; y < EDGE_LENGTH; y++)
				pixels[y * EDGE_LENGTH + x] = ((x % EDGE_LENGTH) ^ (y % EDGE_LENGTH)) ? grey : color;
		m_error_checkerboard_image = create_ref<image>(pixels.data(), EDGE_LENGTH, EDGE_LENGTH, image_format::RGBA);

		VkSamplerCreateInfo sampler{};
		sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	
		sampler.magFilter = VK_FILTER_NEAREST;
		sampler.minFilter = VK_FILTER_NEAREST;
		vkCreateSampler(m_device, &sampler, nullptr, &m_default_sampler_nearest);

		sampler.magFilter = VK_FILTER_LINEAR;
		sampler.minFilter = VK_FILTER_LINEAR;
		vkCreateSampler(m_device, &sampler, nullptr, &m_default_sampler_linear);

		m_deletion_queue.push_pointer(&m_white_image);
		m_deletion_queue.push_pointer(&m_black_image);
		m_deletion_queue.push_pointer(&m_grey_image);
		m_deletion_queue.push_pointer(&m_error_checkerboard_image);
		m_deletion_queue.push_pointer(m_default_sampler_linear);
		m_deletion_queue.push_pointer(m_default_sampler_nearest);

		// =========================================================== DEFAULT SCENE DATA =========================================================== 

		m_scene_data.sunlight_color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		m_scene_data.ambient_color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		m_scene_data.sunlight_direction = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

		// =========================================================== DEFAULT MATERIAL =========================================================== 

		//set the uniform buffer for the material data
		vk_buffer material_constant = create_buffer(sizeof(material::material_constants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		//write the buffer
		material::material_constants* scene_uniform_data = (material::material_constants*)material_constant.allocation->GetMappedData();
		scene_uniform_data->color_factors = glm::vec4{ 1, 1, 1, 1 };
		scene_uniform_data->metal_rough_factors = glm::vec4{ 1, 0.5, 0, 0 };

		m_deletion_queue.push_func([=]() { destroy_buffer(material_constant); });

		material::material_resources material_resources;						//default the material textures
		material_resources.color_image = m_white_image;
		material_resources.color_sampler = m_default_sampler_linear;
		material_resources.metal_rough_image = m_white_image;
		material_resources.metal_rough_sampler = m_default_sampler_linear;
		material_resources.data_buffer = material_constant.buffer;
		material_resources.data_buffer_offset = 0;
		m_default_material = m_metal_rough_material.create_instance(material_pass::main_color, material_resources, m_global_descriptor_allocator);
	}

	void vk_renderer::serialize(const PFF::serializer::option option) {

		PFF::serializer::yaml(config::get_filepath_from_configtype(config::file::engine), "renderer_background_effect", option)
			.entry("current_background_effect", m_current_background_effect)
			.vector(KEY_VALUE(m_background_effects), [=](serializer::yaml& yaml, const u64 x) {
				yaml.entry(KEY_VALUE(m_background_effects[x].name))
				.entry("data_0", m_background_effects[x].data.data1)
				.entry("data_1", m_background_effects[x].data.data2)
				.entry("data_2", m_background_effects[x].data.data3)
				.entry("data_3", m_background_effects[x].data.data4);
			});
	}

	void vk_renderer::init_swapchain() {

		create_swapchain(m_window->get_width(), m_window->get_height());

		//draw image size will match the window
		VkExtent3D drawImageExtent = { 
			m_window->get_width(),
			m_window->get_height(),
			1 
		};
		m_window->get_monitor_size((int*)&drawImageExtent.width, (int*)&drawImageExtent.height);
		CORE_LOG(Trace, "render image extend: " << drawImageExtent.width << "/" << drawImageExtent.height);

		VmaAllocationCreateInfo image_alloc_CI = {};
		image_alloc_CI.usage = VMA_MEMORY_USAGE_GPU_ONLY;												// for the m_draw_image & m_depth_image, allocate it from GPU local memory
		image_alloc_CI.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);		// ensure image is only on GPU (only GPU side VRAM has this flag)

		// =========================================== create draw_image ===========================================

		m_draw_image.set_image_format(VK_FORMAT_R16G16B16A16_SFLOAT);
		m_draw_image.set_image_extent(drawImageExtent);

		VkImageUsageFlags draw_image_usages{};
		draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_STORAGE_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_SAMPLED_BIT;
		//draw_image_usages |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		VkImageCreateInfo image_CI = init::image_create_info(m_draw_image.get_image_format(), draw_image_usages, drawImageExtent);

		vmaCreateImage(m_allocator, &image_CI, &image_alloc_CI, m_draw_image.get_image_pointer(), m_draw_image.get_allocation_pointer(), nullptr);						// allocate and create the image
		VkImageViewCreateInfo view_CI = init::imageview_create_info(m_draw_image.get_image_format(), m_draw_image.get_image(), VK_IMAGE_ASPECT_COLOR_BIT);	// build a image-view for the draw image to use for rendering
		VK_CHECK_S(vkCreateImageView(m_device, &view_CI, nullptr, m_draw_image.get_image_view_pointer()));

		// =========================================== create depth_image ===========================================
		
		m_depth_image.set_image_format(VK_FORMAT_D32_SFLOAT);
		m_depth_image.set_image_extent(drawImageExtent);

		VkImageUsageFlags depthImageUsages{};
		depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		VkImageCreateInfo dimg_info = init::image_create_info(m_depth_image.get_image_format(), depthImageUsages, drawImageExtent);
		
		vmaCreateImage(m_allocator, &dimg_info, &image_alloc_CI, m_depth_image.get_image_pointer(), m_depth_image.get_allocation_pointer(), nullptr);
		VkImageViewCreateInfo dview_info = init::imageview_create_info(m_depth_image.get_image_format(), m_depth_image.get_image(), VK_IMAGE_ASPECT_DEPTH_BIT);
		VK_CHECK_S(vkCreateImageView(m_device, &dview_info, nullptr, m_depth_image.get_image_view_pointer()));

		m_deletion_queue.push_pointer(&m_draw_image);
		m_deletion_queue.push_pointer(&m_depth_image);
	}


	void vk_renderer::init_commands() {

		// create a command pool for commands submitted to the graphics queue.
		// also allow the pool to allow for resetting of individual command buffers
		VkCommandPoolCreateInfo command_pool_CI = init::command_pool_create_info(m_graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		for (int i = 0; i < FRAME_COUNT; i++) {

			VK_CHECK_S(vkCreateCommandPool(m_device, &command_pool_CI, nullptr, &m_frames[i].command_pool));
			VkCommandBufferAllocateInfo command_alloc_I = init::command_buffer_allocate_info(m_frames[i].command_pool, 1);			// allocate default command buffer used for rendering
			VK_CHECK_S(vkAllocateCommandBuffers(m_device, &command_alloc_I, &m_frames[i].main_command_buffer));
		}

		VK_CHECK_S(vkCreateCommandPool(m_device, &command_pool_CI, nullptr, &m_immCommandPool));
		VkCommandBufferAllocateInfo cmdAllocInfo = init::command_buffer_allocate_info(m_immCommandPool, 1);
		VK_CHECK_S(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_immCommandBuffer));

		m_deletion_queue.push_pointer(m_immCommandPool);
	}


	void vk_renderer::init_sync_structures() {

		// one fence to control when the GPU finished rendering the frame,
		// 2 semaphores to syncronize rendering with swapchain
		VkFenceCreateInfo fence_CI = init::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);		// init signaled to avoid thread blocking
		VkSemaphoreCreateInfo semaphore_CI = init::semaphore_create_info();

		for (int i = 0; i < FRAME_COUNT; i++) {

			VK_CHECK_S(vkCreateFence(m_device, &fence_CI, nullptr, &m_frames[i].render_fence));
			VK_CHECK_S(vkCreateSemaphore(m_device, &semaphore_CI, nullptr, &m_frames[i].swapchain_semaphore));
			VK_CHECK_S(vkCreateSemaphore(m_device, &semaphore_CI, nullptr, &m_frames[i].render_semaphore));
		}

		VK_CHECK_S(vkCreateFence(m_device, &fence_CI, nullptr, &m_immFence));
		m_deletion_queue.push_pointer(m_immFence);
	}


	void vk_renderer::init_descriptors() {

		//create a descriptor pool that will hold 10 sets with 1 image each
		std::vector<descriptor_allocator_growable::pool_size_ratio> sizes = {
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
		};
		m_global_descriptor_allocator.init(m_device, 10, sizes);

		{
			//make the descriptor set layout for our compute draw
			descriptor_layout_builder builder;
			builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			builder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_draw_image_descriptor_layout = builder.build(m_device, VK_SHADER_STAGE_COMPUTE_BIT);
			m_draw_image_descriptors = m_global_descriptor_allocator.allocate(m_device, m_draw_image_descriptor_layout);
		}

		{
			descriptor_writer writer;
			writer.write_image(0, m_draw_image.get_image_view(), VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			writer.update_set(m_device, m_draw_image_descriptors);
		}
		
		m_deletion_queue.push_pointer(m_draw_image_descriptor_layout);
		m_deletion_queue.push_pointer(m_single_image_descriptor_layout);
		m_deletion_queue.push_pointer(m_gpu_scene_data_descriptor_layout);
		m_deletion_queue.push_func([&] { m_global_descriptor_allocator.destroy_pools(m_device); });
		
		{
			descriptor_layout_builder builder;
			builder.add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_single_image_descriptor_layout = builder.build(m_device, VK_SHADER_STAGE_FRAGMENT_BIT);
		}

		// =========================================== init frame descriptor pool ===========================================

		for (int i = 0; i < FRAME_COUNT; i++) {

			// create a descriptor pool
			std::vector<descriptor_allocator_growable::pool_size_ratio> frame_sizes = {
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
			};

			m_frames[i].frame_descriptors = descriptor_allocator_growable{};
			m_frames[i].frame_descriptors.init(m_device, 1000, frame_sizes);

			m_deletion_queue.push_func([=]() { m_frames[i].frame_descriptors.destroy_pools(m_device); });
		}

		{
			descriptor_layout_builder builder;
			builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			m_gpu_scene_data_descriptor_layout = builder.build(m_device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

			m_deletion_queue.push_pointer(m_gpu_scene_data_descriptor_layout);
		}
	}


	// ================================================================================ INIT PIPELINES ================================================================================

	void vk_renderer::init_pipelines() {

		// COMPUTE PIPELINES	
		init_pipelines_background();

		// GRAPHICS PIPELINES
		init_pipeline_mesh();

		m_metal_rough_material.build_pipelines();
		m_deletion_queue.push_func([&] { m_metal_rough_material.release_resources(); });
	}

	void vk_renderer::init_pipelines_background() {

		VkPushConstantRange pushConstant{};
		pushConstant.offset = 0;
		pushConstant.size = sizeof(render::compute_push_constants);
		pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkPipelineLayoutCreateInfo computeLayout_CI{};
		computeLayout_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		computeLayout_CI.pNext = nullptr;
		computeLayout_CI.pSetLayouts = &m_draw_image_descriptor_layout;
		computeLayout_CI.setLayoutCount = 1;
		computeLayout_CI.pPushConstantRanges = &pushConstant;
		computeLayout_CI.pushConstantRangeCount = 1;
		VK_CHECK_S(vkCreatePipelineLayout(m_device, &computeLayout_CI, nullptr, &m_gradient_pipeline_layout));

		VkPipelineShaderStageCreateInfo stageinfo{};
		stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageinfo.pNext = nullptr;
		stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		// TIP: giving it the name of the function we want the shader to use => main().
		//		It is possible to store multiple compute shader variants on the same shader file, by using different entry point functions and then setting them up here.
		stageinfo.pName = "main";

		VkComputePipelineCreateInfo compute_pipeline_CI{};
		compute_pipeline_CI.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		compute_pipeline_CI.pNext = nullptr;
		compute_pipeline_CI.layout = m_gradient_pipeline_layout;
		compute_pipeline_CI.stage = stageinfo;

		// ====================================================== Add pipeline [grid] ====================================================== 

		VkShaderModule grid_shader;
		CORE_ASSERT(util::load_shader_module("../PFF/shaders/gradient.comp.spv", m_device, &grid_shader), "", "Error when building the compute shader");
		compute_pipeline_CI.stage.module = grid_shader;		//change the shader module only

		render::compute_effect grid{};
		grid.layout = m_gradient_pipeline_layout;
		grid.name = "grid";

		VK_CHECK_S(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_CI, nullptr, &grid.pipeline));
		m_background_effects.emplace_back(grid);
		vkDestroyShaderModule(m_device, grid_shader, nullptr);

		// ====================================================== Add pipeline [gradient] ====================================================== 		

		VkShaderModule gradient_shader;
		CORE_ASSERT(util::load_shader_module("../PFF/shaders/gradient_color.comp.spv", m_device, &gradient_shader), "", "Error when building the compute shader");
		compute_pipeline_CI.stage.module = gradient_shader;	//change the shader module only

		render::compute_effect gradient{};
		gradient.layout = m_gradient_pipeline_layout;
		gradient.name = "gradient";
		// --------------- default gradient color --------------- 
		gradient.data.data1 = glm::vec4(1.f, 0.f, 0.f, 1.f);
		gradient.data.data2 = glm::vec4(0.f, 0.f, 1.f, 1.f);

		VK_CHECK_S(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_CI, nullptr, &gradient.pipeline));
		m_background_effects.emplace_back(gradient);
		vkDestroyShaderModule(m_device, gradient_shader, nullptr);

		// ====================================================== Add pipeline [sky] ====================================================== 

		VkShaderModule sky_shader;
		CORE_ASSERT(util::load_shader_module("../PFF/shaders/sky.comp.spv", m_device, &sky_shader), "", "Error when building the compute shader");
		compute_pipeline_CI.stage.module = sky_shader;		//change the shader module only

		render::compute_effect sky{};
		sky.layout = m_gradient_pipeline_layout;
		sky.name = "sky";
		// --------------- default sky parameters --------------- 
		sky.data.data1 = glm::vec4(.1f, .2f, .4f, .99f);

		//change the shader module only
		VK_CHECK_S(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_CI, nullptr, &sky.pipeline));
		m_background_effects.emplace_back(sky);
		vkDestroyShaderModule(m_device, sky_shader, nullptr);


		m_deletion_queue.push_func([&]() {

			vkDestroyPipelineLayout(m_device, m_gradient_pipeline_layout, nullptr);
			for (u64 x = 0; x < m_background_effects.size(); x++)
				vkDestroyPipeline(m_device, m_background_effects[x].pipeline, nullptr);
			});
	}


	void vk_renderer::init_pipeline_mesh() {
	
#if 1	// Try new initalization with new shader
		VkShaderModule mesh_frag_shader;
		CORE_ASSERT(util::load_shader_module("../PFF/shaders/tex_image.frag.spv", m_device, &mesh_frag_shader), "Triangle fragment shader succesfully loaded", "Error when building the fragment shader");

		VkShaderModule mesh_vertex_shader;
		CORE_ASSERT(util::load_shader_module("../PFF/shaders/colored_triangle_mesh.vert.spv", m_device, &mesh_vertex_shader), "Triangle vertex shader succesfully loaded", "Error when building the vertex shader");

		VkPushConstantRange bufferRange{};
		bufferRange.offset = 0;
		bufferRange.size = sizeof(GPU_draw_push_constants);
		bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineLayoutCreateInfo pipeline_layout_info = init::pipeline_layout_create_info();
		pipeline_layout_info.pPushConstantRanges = &bufferRange;
		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pSetLayouts = &m_single_image_descriptor_layout;
		pipeline_layout_info.setLayoutCount = 1;
		VK_CHECK_S(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_mesh_pipeline_layout));
#else
		VkShaderModule mesh_frag_shader;
		CORE_ASSERT(util::load_shader_module("../PFF/shaders/colored_triangle.frag.spv", m_device, &mesh_frag_shader), "", "Error when building the triangle fragment shader module");

		VkShaderModule mesh_vertex_shader;
		CORE_ASSERT(util::load_shader_module("../PFF/shaders/colored_triangle_mesh.vert.spv", m_device, &mesh_vertex_shader), "", "Error when building the triangle vertex shader module");

		VkPushConstantRange push_constant_range{};
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(GPU_draw_push_constants);
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineLayoutCreateInfo pipeline_layout_info = init::pipeline_layout_create_info();
		pipeline_layout_info.pPushConstantRanges = &push_constant_range;
		pipeline_layout_info.pushConstantRangeCount = 1;
		VK_CHECK_S(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_mesh_pipeline_layout));
#endif

		m_mesh_pipeline = pipeline_builder()
			.set_pipeline_layout(m_mesh_pipeline_layout)				// use the triangle layout we created
			.set_shaders(mesh_vertex_shader, mesh_frag_shader)			// connecting the vertex and pixel shaders to the pipeline
			.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)	// it will draw triangles
			.set_polygon_mode(VK_POLYGON_MODE_FILL)						// filled triangles
			.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)	// no backface culling
			.set_multisampling_none()									// no multisampling
			//.disable_blending()										// no blending
			.enable_blending_additive()
			//.disable_depthtest()										
			.enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL)
			.set_color_attachment_format(m_draw_image.get_image_format())		// connect the image format we will draw into, from draw image
			.set_depth_format(m_depth_image.get_image_format())
			.build(m_device);

		//clean structures
		vkDestroyShaderModule(m_device, mesh_frag_shader, nullptr);
		vkDestroyShaderModule(m_device, mesh_vertex_shader, nullptr);

		//m_deletion_queue.push_pointer(m_single_image_descriptor_layout);
		//m_deletion_queue.push_pointer(m_mesh_pipeline_layout);
		//m_deletion_queue.push_pointer(m_mesh_pipeline);

		m_deletion_queue.push_func([&]() {

			vkDestroyDescriptorSetLayout(m_device, m_single_image_descriptor_layout, nullptr);
			vkDestroyPipelineLayout(m_device, m_mesh_pipeline_layout, nullptr);
			vkDestroyPipeline(m_device, m_mesh_pipeline, nullptr);
		});
	}

	// ================================================================================== PIPELINES ==================================================================================

	void vk_renderer::destroy_swapchain() {

		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

		// destroy swapchain resources
		for (u64 i = 0; i < m_swapchain_image_views.size(); i++)
			vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);

	}

	void vk_renderer::resize_swapchain() {

		vkDeviceWaitIdle(m_device);

		destroy_swapchain();
		m_swapchain_extent.width = m_window->get_width();
		m_swapchain_extent.height = m_window->get_height();
		create_swapchain(m_swapchain_extent.width, m_swapchain_extent.height);

		m_resize_nedded = false;
	}


	void vk_renderer::draw_internal(VkCommandBuffer cmd) {

		render::compute_effect& effect = m_background_effects[m_current_background_effect];

		// bind the gradient drawing compute pipeline
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_gradient_pipeline_layout, 0, 1, &m_draw_image_descriptors, 0, nullptr);		// bind desc_set containing the draw_image for compute pipeline
		vkCmdPushConstants(cmd, m_gradient_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(render::compute_push_constants), &effect.data);

		// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
		vkCmdDispatch(cmd, static_cast<u32>(std::ceil(m_draw_extent.width / 16.0)), static_cast<u32>(std::ceil(m_draw_extent.height / 16.0)), 1);
	}

	void vk_renderer::draw_geometry(VkCommandBuffer cmd) {

		// ========================================== create GPU global scene data ==========================================

		//set dynamic viewport and scissor
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<f32>(m_draw_extent.width);
		viewport.height = static_cast<f32>(m_draw_extent.height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		vkCmdSetViewport(cmd, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = m_draw_extent.width;
		scissor.extent.height = m_draw_extent.height;
		vkCmdSetScissor(cmd, 0, 1, &scissor);

		m_scene_data.view = m_active_camera->get_view();
		m_scene_data.proj = glm::perspective(glm::radians(m_active_camera->get_perspective_fov_y()), (float)m_draw_extent.width / (float)m_draw_extent.height, 100000.f, 0.1f);
		m_scene_data.proj[1][1] *= -1;				// invert the Y direction on projection matrix
		m_scene_data.proj_view = m_scene_data.proj * m_scene_data.view;

		// allocate a new uniform buffer for the scene data
		vk_buffer gpuSceneDataBuffer = create_buffer(sizeof(render::GPU_scene_data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		//add it to the deletion queue of this frame so it gets deleted once its been used
		get_current_frame().deletion_queue.push_func([=]() { destroy_buffer(gpuSceneDataBuffer); });

		//write the buffer
		render::GPU_scene_data* scene_uniform_data = (render::GPU_scene_data*)gpuSceneDataBuffer.allocation->GetMappedData();
		*scene_uniform_data = m_scene_data;

		//create a descriptor set that binds that buffer and update it
		VkDescriptorSet globalDescriptor = get_current_frame().frame_descriptors.allocate(m_device, m_gpu_scene_data_descriptor_layout);
		descriptor_writer writer;
		writer.write_buffer(0, gpuSceneDataBuffer.buffer, sizeof(render::GPU_scene_data), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		writer.update_set(m_device, globalDescriptor);

		VkRenderingAttachmentInfo color_attachment = init::attachment_info(m_draw_image.get_image_view(), nullptr, VK_IMAGE_LAYOUT_GENERAL);					// begin a render pass connected to our draw image
		VkRenderingAttachmentInfo depth_attachment = init::depth_attachment_info(m_depth_image.get_image_view(), VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
		VkRenderingInfo render_info = init::rendering_info(m_draw_extent, &color_attachment, &depth_attachment);

		vkCmdBeginRendering(cmd, &render_info);

		m_renderer_metrik.reset();

#define PROCCESS 0
#if PROCCESS == 0

		/*
			PROCCESS V0
			1. get world_layer
			2. get active_maps from world_layer
			3. get all mesh components
			4. draw all mesh components

			pro:
				all code for rendering is in one place
			
			con:
		 
		*/

		// loop over all maps in worlds
		auto& all_maps = application::get().get_world_layer()->get_maps();
		for (ref<map> loc_map : all_maps) {

			if (!loc_map->is_active())		// skip maps that are not-loaded/hidden/disabled
				continue;

			// TODO: add high-level culling for maps that don't need rendering
				// oclution culling
				// frustum culling

			// get every entity with [transform] and [mesh]
			auto group = loc_map->get_registry().group<transform_component>(entt::get<mesh_component>);
			for (auto entity : group) {
				auto& [transform, mesh_comp] = group.get<transform_component, mesh_component>(entity);

				// TODO: add culling for geometry that doesn't need to be drawns
					// oclution culling

#if 1
				if (!is_bounds_in_frustum(m_scene_data.proj_view, mesh_comp.mesh_asset->bounds, (glm::mat4)transform))
					continue;
#endif

#if 0
				material_instance* loc_material = (mesh_comp.material != nullptr) ? mesh_comp.material : &m_default_material;
#else
				material_instance* loc_material = &m_default_material;
#endif
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, loc_material->pipeline->pipeline);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, loc_material->pipeline->layout, 0, 1, &globalDescriptor, 0, nullptr);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, loc_material->pipeline->layout, 1, 1, &loc_material->material_set, 0, nullptr);
				vkCmdBindIndexBuffer(cmd, mesh_comp.mesh_asset->mesh_buffers.index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

				GPU_draw_push_constants push_constants;
				push_constants.world_matrix = (glm::mat4)transform * mesh_comp.transform;
				push_constants.vertex_buffer = mesh_comp.mesh_asset->mesh_buffers.vertex_buffer_address;
				vkCmdPushConstants(cmd, mesh_comp.material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPU_draw_push_constants), &push_constants);

				// Draw every surface in mesh_asset
				for (u64 x = 0; x < mesh_comp.mesh_asset->surfaces.size(); x++) {

					vkCmdDrawIndexed(cmd, mesh_comp.mesh_asset->surfaces[x].count, 1, mesh_comp.mesh_asset->surfaces[x].startIndex, 0, 0);

					m_renderer_metrik.vertecies += (u64)mesh_comp.mesh_asset->surfaces[x].count / 3;
					m_renderer_metrik.draw_calls++;
				}
				m_renderer_metrik.mesh_draw++;
			}
		}

#elif PROCCESS == 1

		/*
			PROCCESS V1
			1. get world_layer
			2. get active_maps from world_layer
			3. get all entities from registry in active_maps
			4. draw all mesh_components of all entities

			pro:
				all code for rendering is in one place


			con:
		*/

		auto& all_maps = application::get().get_world_layer()->get_maps();
		for (ref<map> loc_map : all_maps) {

			if (!loc_map->is_active())		// skip maps that are not-loaded/hidden
				continue;


		}

#elif PROCCESS == 2

		/*
			PROCCESS V2
			1. call world_layer function from renderer to submit meshes
			2. world_layer delegates the call to any map it wants to draw
			3. map submits all meshes to renderer

			pro:
				lets every map individually decide what to render

			con:
				a lot more function calls
				rendering code is scatered through code_base

			application::get().get_world_layer()->notefy_maps_to_render();

			bulk of implementation would be in [world_lsyer] and [maps]
		*/

#endif
	
		vkCmdEndRendering(cmd);

	}

	bool vk_renderer::is_bounds_in_frustum(const glm::mat4& pro_view, const PFF::geometry::bounds& bounds, const glm::mat4& transform) {

		std::array<glm::vec3, 8> corners{
			glm::vec3 { 1, 1, 1 },
			glm::vec3 { 1, 1, -1 },
			glm::vec3 { 1, -1, 1 },
			glm::vec3 { 1, -1, -1 },
			glm::vec3 { -1, 1, 1 },
			glm::vec3 { -1, 1, -1 },
			glm::vec3 { -1, -1, 1 },
			glm::vec3 { -1, -1, -1 },
		};

		// projection * view

		glm::mat4 matrix = pro_view * transform;
		glm::vec3 min = { 1.5, 1.5, 1.5 };
		glm::vec3 max = { -1.5, -1.5, -1.5 };

		for (int x = 0; x < 8; x++) {
			glm::vec4 v = matrix * glm::vec4(bounds.origin + (corners[x] * bounds.extents), 1.f);	// project each corner into clip space

			// perspective correction
			v.x = v.x / v.w;
			v.y = v.y / v.w;
			v.z = v.z / v.w;

			min = glm::min(glm::vec3{ v.x, v.y, v.z }, min);
			max = glm::max(glm::vec3{ v.x, v.y, v.z }, max);
		}

		// check the clip space box is within the view
		return !(min.z > 1.f || max.z < 0.f || min.x > 1.f || max.x < -1.f || min.y > 1.f || max.y < -1.f);
	}

	void vk_renderer::draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView) {

		VkRenderingAttachmentInfo colorAttachment = init::attachment_info(targetImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
		VkRenderingInfo renderInfo = init::rendering_info(m_swapchain_extent, &colorAttachment, nullptr);

		vkCmdBeginRendering(cmd, &renderInfo);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
		vkCmdEndRendering(cmd);
	}

	void vk_renderer::create_swapchain(u32 width, u32 height) {
				
		m_swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;
		VkPresentModeKHR present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
		vkb::Swapchain vkbSwapchain = vkb::SwapchainBuilder( m_chosenGPU, m_device, m_surface )
			//.use_default_format_selection()
			.set_desired_format(VkSurfaceFormatKHR{ m_swapchain_image_format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
			// VK_PRESENT_MODE_MAILBOX_KHR => fastest mode
			// VK_PRESENT_MODE_FIFO_KHR => vsync present mode
			.set_desired_present_mode(present_mode)
			.set_desired_extent(width, height)
			.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.build().value();

		m_swapchain_extent = vkbSwapchain.extent;
		m_swapchain = vkbSwapchain.swapchain;
		m_swapchain_images = vkbSwapchain.get_images().value();
		m_swapchain_image_views = vkbSwapchain.get_image_views().value();
	}

	// =======================================================================================================================================
	// BUFFER
	// =======================================================================================================================================

	vk_buffer vk_renderer::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {

		VkBufferCreateInfo buffer_CI = {};
		buffer_CI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_CI.pNext = nullptr;
		buffer_CI.size = allocSize;
		buffer_CI.usage = usage;

		VmaAllocationCreateInfo vmaalloc_CI = {};
		vmaalloc_CI.usage = memoryUsage;
		vmaalloc_CI.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
		vk_buffer new_buffer;

		VK_CHECK_S(vmaCreateBuffer(m_allocator, &buffer_CI, &vmaalloc_CI, &new_buffer.buffer, &new_buffer.allocation, &new_buffer.info));
		return new_buffer;
	}


	void vk_renderer::destroy_buffer(const vk_buffer& buffer) { vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation); }


	render::GPU_mesh_buffers vk_renderer::upload_mesh(std::vector<u32> indices, std::vector<PFF::geometry::vertex> vertices) {

		const size_t vertexBufferSize = vertices.size() * sizeof(PFF::geometry::vertex);
		const size_t indexBufferSize = indices.size() * sizeof(u32);
		render::GPU_mesh_buffers new_mesh{};
		new_mesh.vertex_buffer = create_buffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		
		//find the adress of the vertex buffer
		VkBufferDeviceAddressInfo device_adress_I{};
		device_adress_I.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		device_adress_I.buffer = new_mesh.vertex_buffer.buffer;

		new_mesh.vertex_buffer_address = vkGetBufferDeviceAddress(m_device, &device_adress_I);
		new_mesh.index_buffer = create_buffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);		
		vk_buffer staging = create_buffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* data = staging.allocation->GetMappedData();
		
		memcpy(data, vertices.data(), vertexBufferSize);								// copy vertex buffer
		memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);		// copy index buffer

		immediate_submit([&](VkCommandBuffer cmd) {

			VkBufferCopy vertexCopy{ 0 };
			vertexCopy.dstOffset = 0;
			vertexCopy.srcOffset = 0;
			vertexCopy.size = vertexBufferSize;
			vkCmdCopyBuffer(cmd, staging.buffer, new_mesh.vertex_buffer.buffer, 1, &vertexCopy);

			VkBufferCopy indexCopy{ 0 };
			indexCopy.dstOffset = 0;
			indexCopy.srcOffset = vertexBufferSize;
			indexCopy.size = indexBufferSize;
			vkCmdCopyBuffer(cmd, staging.buffer, new_mesh.index_buffer.buffer, 1, &indexCopy);
		});

		destroy_buffer(staging);
		m_deletion_queue.push_func([=]() {

			destroy_buffer(new_mesh.vertex_buffer);
			destroy_buffer(new_mesh.index_buffer);
		});
		
		return new_mesh;
	}

}
