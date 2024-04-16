
#include "util/pffpch.h"

//#define VK_NO_PROTOTYPES
//#include "vendor/volk.h"
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

#include "GLFW/glfw3.h"
#include "application.h"
#include "util/io/serializer.h"
#include "util/imgui/imgui_util.h"
#include "util/color_theme.h"
#include "engine/layer/layer_stack.h"
#include "engine/layer/layer.h"

#include "vk_renderer.h"

namespace PFF::render::vulkan {

	// ============================================= setup  ============================================= 

	vk_renderer::vk_renderer(ref<pff_window> window, ref<PFF::layer_stack> layer_stack)
		: m_window(window), m_layer_stack(layer_stack) {

		CORE_LOG_INIT();

		vkb::InstanceBuilder builder;

		//make the vulkan instance, with basic debug features
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

		m_deletion_queue.push_func([&]() { vmaDestroyAllocator(m_allocator); });

		init_commands();
		init_swapchain();
		init_sync_structures();
		init_descriptors();
		
		init_pipelines();
		imgui_init();

		m_is_initialized = true;
	}

	vk_renderer::~vk_renderer() { 

		if (!m_is_initialized)
			return;

		vkDeviceWaitIdle(m_device);
		imgui_shutdown();
		
		m_deletion_queue.flush();
		for (int i = 0; i < FRAME_COUNT; i++) {

			vkDestroyCommandPool(m_device, m_frames[i].command_pool, nullptr);

			vkDestroyFence(m_device, m_frames[i].render_fence, nullptr);
			vkDestroySemaphore(m_device, m_frames[i].render_semaphore, nullptr);
			vkDestroySemaphore(m_device, m_frames[i].swapchain_semaphore, nullptr);

			m_frames->deletion_queue.flush();
		}
		
		destroy_swapchain();
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkDestroyDevice(m_device, nullptr);
		vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
		vkDestroyInstance(m_instance, nullptr);
		
		m_window.reset();
		m_layer_stack.reset();

		CORE_LOG_SHUTDOWN();
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// PUBLIC FUNCTIONS
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
		VK_CHECK(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_imgui_desc_pool));

		// init imgui library
		IMGUI_CHECKVERSION();
		PFF::UI::imgui::util::s_context = ImGui::CreateContext();

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
		io.IniFilename = "./config/imgui.ini";
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

		// Load fonts
		//ImGuiIO& io = ImGui::GetIO();
		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = true;

		PFF::UI::imgui::util::s_fonts["default"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Regular.ttf", 15.f);
		PFF::UI::imgui::util::s_fonts["bold"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Bold.ttf", 15.f);
		PFF::UI::imgui::util::s_fonts["italic"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Italic.ttf", 15.f);
		
		PFF::UI::imgui::util::s_fonts["regular_big"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Regular.ttf", 18.f);
		PFF::UI::imgui::util::s_fonts["bold_big"] =	io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Bold.ttf", 18.f);
		PFF::UI::imgui::util::s_fonts["italic_big"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Italic.ttf", 18.f);
		
		PFF::UI::imgui::util::s_fonts["giant"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Bold.ttf", 30.f);
		io.FontDefault = PFF::UI::imgui::util::s_fonts["default"];
		
		// execute a gpu command to upload imgui font textures
		//ImGui_ImplVulkan_CreateFontsTexture();
		immediate_submit([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); });

		serializer::yaml(config::get_filepath_from_configtype(config::file::editor), "UI", serializer::option::load_from_file)
			.entry(KEY_VALUE(UI::THEME::main_color))
			.entry(KEY_VALUE(UI::THEME::UI_theme));
		UI::THEME::update_UI_theme();

		m_imgui_initalized = true;

		m_deletion_queue.push_func([=]() {

			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext(PFF::UI::imgui::util::s_context);
			vkDestroyDescriptorPool(m_device, m_imgui_desc_pool, nullptr);

			m_imgui_initalized = false;
		});
	}

	void vk_renderer::imgui_shutdown() { 
		
	}

	void vk_renderer::draw_frame(f32 delta_time) {

		if (m_state != system_state::active)
			return;

		VK_CHECK(vkWaitForFences(m_device, 1, &get_current_frame().render_fence, true, 1000000000));

		get_current_frame().deletion_queue.flush();
		uint32_t swapchain_image_index;
		VkResult e = vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, get_current_frame().swapchain_semaphore, nullptr, &swapchain_image_index);
		if (e == VK_ERROR_OUT_OF_DATE_KHR) {
			//rebuild_swapchain();		// TODO: implement rebuild
			return;
		}

		VK_CHECK(vkResetFences(m_device, 1, &get_current_frame().render_fence));

		VkCommandBuffer cmd = get_current_frame().main_command_buffer;
		VK_CHECK(vkResetCommandBuffer(cmd, 0));

		VkCommandBufferBeginInfo cmdBeginInfo = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		m_draw_extent.width = m_draw_image.image_extent.width;
		m_draw_extent.height = m_draw_image.image_extent.height;

		VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		util::transition_image(cmd, m_draw_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		draw_internal(cmd);

		//transition the draw image and the swapchain image into their correct transfer layouts
		util::transition_image(cmd, m_draw_image.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		util::copy_image_to_image(cmd, m_draw_image.image, m_swapchain_images[swapchain_image_index], m_draw_extent, m_swapchain_extent);						// copy from draw_image into swapchain

		// =========================================================== draw imgui ===========================================================
		if (m_imgui_initalized) {
		
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			for (layer* layer : *m_layer_stack) {

				// LOG(Trace, "drawing imgui of layer [" << layer->get_name() << "]");
				layer->on_imgui_render();
			}

			ImGui::EndFrame();
			ImGui::Render();

			// update other platform windows
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);

			util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			draw_imgui(cmd, m_swapchain_image_views[swapchain_image_index]);		//draw imgui into the swapchain image
			util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		}
		// =========================================================== draw imgui ===========================================================

		else
			util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
				
		VK_CHECK(vkEndCommandBuffer(cmd));

		VkCommandBufferSubmitInfo cmdinfo = init::command_buffer_submit_info(cmd);
		VkSemaphoreSubmitInfo waitInfo = init::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, get_current_frame().swapchain_semaphore);
		VkSemaphoreSubmitInfo signalInfo = init::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame().render_semaphore);
		VkSubmitInfo2 submit = init::submit_info(&cmdinfo, &signalInfo, &waitInfo);
		VK_CHECK(vkQueueSubmit2(m_graphics_queue, 1, &submit, get_current_frame().render_fence));		// [m_render_fence] will now block until the graphic commands finish execution

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_swapchain;
		presentInfo.swapchainCount = 1;
		presentInfo.pWaitSemaphores = &get_current_frame().render_semaphore;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pImageIndices = &swapchain_image_index;
		VK_CHECK(vkQueuePresentKHR(m_graphics_queue, &presentInfo));

		m_frame_number++;
	}

	void vk_renderer::refresh(f32 delta_time) {}

	void vk_renderer::set_size(u32 width, u32 height) {}

	void vk_renderer::wait_idle() { vkDeviceWaitIdle(m_device); }

	void vk_renderer::immediate_submit(std::function<void()>&& function) {

		VK_CHECK(vkResetFences(m_device, 1, &m_immFence));
		VK_CHECK(vkResetCommandBuffer(m_immCommandBuffer, 0));

		VkCommandBuffer cmd = m_immCommandBuffer;
		VkCommandBufferBeginInfo cmdBeginInfo = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		function();

		VK_CHECK(vkEndCommandBuffer(cmd));
		VkCommandBufferSubmitInfo cmdinfo = init::command_buffer_submit_info(cmd);
		VkSubmitInfo2 submit = init::submit_info(&cmdinfo, nullptr, nullptr);

		// submit command buffer to the queue and execute it.
		//  _renderFence will now block until the graphic commands finish execution
		VK_CHECK(vkQueueSubmit2(m_graphics_queue, 1, &submit, m_immFence));
		VK_CHECK(vkWaitForFences(m_device, 1, &m_immFence, true, 9999999999));
	}

	void vk_renderer::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function) {

		VK_CHECK(vkResetFences(m_device, 1, &m_immFence));
		VK_CHECK(vkResetCommandBuffer(m_immCommandBuffer, 0));

		VkCommandBuffer cmd = m_immCommandBuffer;
		VkCommandBufferBeginInfo cmdBeginInfo = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		function(cmd);

		VK_CHECK(vkEndCommandBuffer(cmd));
		VkCommandBufferSubmitInfo cmdinfo = init::command_buffer_submit_info(cmd);
		VkSubmitInfo2 submit = init::submit_info(&cmdinfo, nullptr, nullptr);

		// submit command buffer to the queue and execute it.
		//  _renderFence will now block until the graphic commands finish execution
		VK_CHECK(vkQueueSubmit2(m_graphics_queue, 1, &submit, m_immFence));
		VK_CHECK(vkWaitForFences(m_device, 1, &m_immFence, true, 9999999999));
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// PRIVATE FUNCTIONS
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void vk_renderer::init_swapchain() {

		create_swapchain(m_window->get_width(), m_window->get_height());

		//draw image size will match the window
		VkExtent3D drawImageExtent = {
			m_window->get_width(),
			m_window->get_height(),
			1
		};

		m_draw_image.image_format = VK_FORMAT_R16G16B16A16_SFLOAT;
		m_draw_image.image_extent = drawImageExtent;

		VkImageUsageFlags draw_image_usages{};
		draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_STORAGE_BIT;
		draw_image_usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VkImageCreateInfo image_CI = init::image_create_info(m_draw_image.image_format, draw_image_usages, drawImageExtent);

		VmaAllocationCreateInfo image_alloc_CI = {};
		image_alloc_CI.usage = VMA_MEMORY_USAGE_GPU_ONLY;												// for the m_draw_image, we want to allocate it from GPU local memory
		image_alloc_CI.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);		// ensure image is only on GPU (only GPU side VRAM has this flag

		vmaCreateImage(m_allocator, &image_CI, &image_alloc_CI, &m_draw_image.image, &m_draw_image.allocation, nullptr);						// allocate and create the image
		VkImageViewCreateInfo view_CI = init::imageview_create_info(m_draw_image.image_format, m_draw_image.image, VK_IMAGE_ASPECT_COLOR_BIT);	// build a image-view for the draw image to use for rendering

		VK_CHECK(vkCreateImageView(m_device, &view_CI, nullptr, &m_draw_image.image_view));

		//add to deletion queues
		m_deletion_queue.push_func([=]() {

			vkDestroyImageView(m_device, m_draw_image.image_view, nullptr);
			vmaDestroyImage(m_allocator, m_draw_image.image, m_draw_image.allocation);
			});
	}


	void vk_renderer::init_commands() {

		// create a command pool for commands submitted to the graphics queue.
		// also allow the pool to allow for resetting of individual command buffers
		VkCommandPoolCreateInfo command_pool_CI = init::command_pool_create_info(m_graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		for (int i = 0; i < FRAME_COUNT; i++) {

			VK_CHECK(vkCreateCommandPool(m_device, &command_pool_CI, nullptr, &m_frames[i].command_pool));
			VkCommandBufferAllocateInfo command_alloc_I = init::command_buffer_allocate_info(m_frames[i].command_pool, 1);			// allocate default command buffer used for rendering
			VK_CHECK(vkAllocateCommandBuffers(m_device, &command_alloc_I, &m_frames[i].main_command_buffer));
		}

		VK_CHECK(vkCreateCommandPool(m_device, &command_pool_CI, nullptr, &m_immCommandPool));
		VkCommandBufferAllocateInfo cmdAllocInfo = init::command_buffer_allocate_info(m_immCommandPool, 1);
		VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_immCommandBuffer));

		m_deletion_queue.push_func([=]() { vkDestroyCommandPool(m_device, m_immCommandPool, nullptr); });
	}


	void vk_renderer::init_sync_structures() {

		// one fence to control when the GPU finished rendering the frame,
		// 2 semaphores to syncronize rendering with swapchain
		VkFenceCreateInfo fence_CI = init::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);		// init signaled to avoid thread blocking
		VkSemaphoreCreateInfo semaphore_CI = init::semaphore_create_info();

		for (int i = 0; i < FRAME_COUNT; i++) {

			VK_CHECK(vkCreateFence(m_device, &fence_CI, nullptr, &m_frames[i].render_fence));
			VK_CHECK(vkCreateSemaphore(m_device, &semaphore_CI, nullptr, &m_frames[i].swapchain_semaphore));
			VK_CHECK(vkCreateSemaphore(m_device, &semaphore_CI, nullptr, &m_frames[i].render_semaphore));
		}

		VK_CHECK(vkCreateFence(m_device, &fence_CI, nullptr, &m_immFence));
		m_deletion_queue.push_func([=]() { vkDestroyFence(m_device, m_immFence, nullptr); });
	}


	void vk_renderer::init_descriptors() {

		//create a descriptor pool that will hold 10 sets with 1 image each
		std::vector<descriptor_allocator::pool_size_ratio> sizes = {
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
		};
		global_descriptor_allocator.init_pool(m_device, 10, sizes);

		//make the descriptor set layout for our compute draw
		descriptor_layout_builder builder;
		builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		m_draw_image_descriptor_layout = builder.build(m_device, VK_SHADER_STAGE_COMPUTE_BIT);
		m_draw_image_descriptors = global_descriptor_allocator.allocate(m_device, m_draw_image_descriptor_layout);

		VkDescriptorImageInfo descr_image_I{};
		descr_image_I.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		descr_image_I.imageView = m_draw_image.image_view;

		VkWriteDescriptorSet drawImageWrite = {};
		drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		drawImageWrite.pNext = nullptr;
		drawImageWrite.dstBinding = 0;
		drawImageWrite.dstSet = m_draw_image_descriptors;
		drawImageWrite.descriptorCount = 1;
		drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		drawImageWrite.pImageInfo = &descr_image_I;
		vkUpdateDescriptorSets(m_device, 1, &drawImageWrite, 0, nullptr);

		m_deletion_queue.push_func([=]() { 
			
			global_descriptor_allocator.clear_descriptors(m_device);
			global_descriptor_allocator.destroy_pool(m_device);
			vkDestroyDescriptorSetLayout(m_device, m_draw_image_descriptor_layout, nullptr); 
			});
	}


	void vk_renderer::init_pipelines() {

		VkPipelineLayoutCreateInfo computeLayout_CI{};
		computeLayout_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		computeLayout_CI.pNext = nullptr;
		computeLayout_CI.pSetLayouts = &m_draw_image_descriptor_layout;
		computeLayout_CI.setLayoutCount = 1;

		VK_CHECK(vkCreatePipelineLayout(m_device, &computeLayout_CI, nullptr, &m_gradient_pipeline_layout));

		VkShaderModule computeDrawShader;
		CORE_ASSERT(util::load_shader_module("shaders/gradient.comp.spv", m_device, &computeDrawShader), "", "Error when building the compute shader");

		VkPipelineShaderStageCreateInfo stageinfo{};
		stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageinfo.pNext = nullptr;
		stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageinfo.module = computeDrawShader;
		// TIP: giving it the name of the function we want the shader to use => main().
		//		It is possible to store multiple compute shader variants on the same shader file, by using different entry point functions and then setting them up here.
		stageinfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.layout = m_gradient_pipeline_layout;
		computePipelineCreateInfo.stage = stageinfo;

		VK_CHECK(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_gradient_pipeline));

		vkDestroyShaderModule(m_device, computeDrawShader, nullptr);
		m_deletion_queue.push_func([&]() {

			vkDestroyPipelineLayout(m_device, m_gradient_pipeline_layout, nullptr);
			vkDestroyPipeline(m_device, m_gradient_pipeline, nullptr);
			});
	}

	void vk_renderer::destroy_swapchain() {

		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

		// destroy swapchain resources
		for (int i = 0; i < m_swapchain_image_views.size(); i++)
			vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);

	}

	void vk_renderer::draw_internal(VkCommandBuffer cmd) {

		////make a clear-color from frame number. This will flash with a 120 frame period.
		//float flash = abs(sin(m_frame_number / 120.f));
		//VkClearColorValue clearValue = { { 0.0f, 0.0f, flash, 1.0f } };
		//VkImageSubresourceRange clearRange = init::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_gradient_pipeline);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_gradient_pipeline_layout, 0, 1, &m_draw_image_descriptors, 0, nullptr);
		vkCmdDispatch(cmd, static_cast<u32>(std::ceil(m_draw_extent.width / 16.0)), static_cast<u32>(std::ceil(m_draw_extent.height / 16.0)), 1);
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

		vkb::Swapchain vkbSwapchain = vkb::SwapchainBuilder( m_chosenGPU, m_device, m_surface )
			//.use_default_format_selection()
			.set_desired_format(VkSurfaceFormatKHR{ m_swapchain_image_format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)			//use vsync present mode
			.set_desired_extent(width, height)
			.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.build()
			.value();

		m_swapchain_extent = vkbSwapchain.extent;
		m_swapchain = vkbSwapchain.swapchain;
		m_swapchain_images = vkbSwapchain.get_images().value();
		m_swapchain_image_views = vkbSwapchain.get_image_views().value();
	}


}
