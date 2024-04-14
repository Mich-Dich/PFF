
#include "util/pffpch.h"

#include "vendor/VkBootstrap.h"

#include "vk_instance.h"
#include "vk_types.h"
#include "vk_initializers.h"
#include "vk_image.h"

#include "vk_renderer.h"

namespace PFF::render::vulkan {

	// ============================================= setup  ============================================= 

	vk_renderer::vk_renderer(ref<pff_window> window)
		: m_window(window) {

		vkb::InstanceBuilder builder;

		//make the vulkan instance, with basic debug features
		auto inst_ret = builder.set_app_name("PFF_GameEngine")
			.request_validation_layers(true)
			.use_default_debug_messenger()
			.set_minimum_instance_version(1, 3, 0)
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

		create_swapchain(m_window->get_width(), m_window->get_height());
		
		// use vkbootstrap to get a Graphics queue
		m_graphics_queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		m_graphics_queue_family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

		init_commands();
		init_sync_structures();

		m_is_initialized = true;
	}

	vk_renderer::~vk_renderer() { 

		if (!m_is_initialized)
			return;

		vkDeviceWaitIdle(m_device);
		
		for (int i = 0; i < FRAME_COUNT; i++) {

			vkDestroyCommandPool(m_device, m_frames[i].command_pool, nullptr);

			vkDestroyFence(m_device, m_frames[i].render_fence, nullptr);
			vkDestroySemaphore(m_device, m_frames[i].render_semaphore, nullptr);
			vkDestroySemaphore(m_device, m_frames[i].swapchain_semaphore, nullptr);
		}
		
		destroy_swapchain();
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkDestroyDevice(m_device, nullptr);
		vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
		vkDestroyInstance(m_instance, nullptr);

		m_window.reset();
	}

	// ============================================= functions ============================================= 

	void vk_renderer::destroy_swapchain() {

		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

		// destroy swapchain resources
		for (int i = 0; i < m_swapchain_image_views.size(); i++)
			vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);

	}

	void vk_renderer::create_swapchain(u32 width, u32 height) {

		vkb::SwapchainBuilder swapchainBuilder{ m_chosenGPU, m_device, m_surface };
		
		m_swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;

		vkb::Swapchain vkbSwapchain = swapchainBuilder
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

	void vk_renderer::init_commands() {

		// create a command pool for commands submitted to the graphics queue.
		// also allow the pool to allow for resetting of individual command buffers
		VkCommandPoolCreateInfo command_pool_CI = init::command_pool_create_info(m_graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		for (int i = 0; i < FRAME_COUNT; i++) {

			VK_CHECK(vkCreateCommandPool(m_device, &command_pool_CI, nullptr, &m_frames[i].command_pool));
			VkCommandBufferAllocateInfo command_alloc_I = init::command_buffer_allocate_info(m_frames[i].command_pool, 1);			// allocate default command buffer used for rendering
			VK_CHECK(vkAllocateCommandBuffers(m_device, &command_alloc_I, &m_frames[i].main_command_buffer));
		}
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
	}

	void vk_renderer::draw_frame(f32 delta_time) {
	
		VK_CHECK(vkWaitForFences(m_device, 1, &get_current_frame().render_fence, true, 1000000000));
		VK_CHECK(vkResetFences(m_device, 1, &get_current_frame().render_fence));

		uint32_t swapchain_image_index;
		VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, get_current_frame().swapchain_semaphore, nullptr, &swapchain_image_index));

		//naming it cmd for shorter writing
		VkCommandBuffer cmd = get_current_frame().main_command_buffer;

		// now that we are sure that the commands finished executing, we can safely
		// reset the command buffer to begin recording again.
		VK_CHECK(vkResetCommandBuffer(cmd, 0));

		//begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
		VkCommandBufferBeginInfo cmdBeginInfo = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		//start the command buffer recording
		VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		//make the swapchain image into writeable mode before rendering
		util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		//make a clear-color from frame number. This will flash with a 120 frame period.
		VkClearColorValue clearValue;
		float flash = abs(sin(m_frame_number / 120.f));
		clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

		VkImageSubresourceRange clearRange = init::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdClearColorImage(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);		//clear image
		util::transition_image(cmd, m_swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);	//make the swapchain image into presentable mode

		VK_CHECK(vkEndCommandBuffer(cmd));		//finalize the command buffer (we can no longer add commands, but it can now be executed)
		
		//prepare the submission to the queue. 
		//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
		//we will signal the _renderSemaphore, to signal that rendering has finished
		VkCommandBufferSubmitInfo cmdinfo = init::command_buffer_submit_info(cmd);
		VkSemaphoreSubmitInfo waitInfo = init::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, get_current_frame().swapchain_semaphore);
		VkSemaphoreSubmitInfo signalInfo = init::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame().render_semaphore);
		VkSubmitInfo2 submit = init::submit_info(&cmdinfo, &signalInfo, &waitInfo);

		//submit command buffer to the queue and execute it.
		// _renderFence will now block until the graphic commands finish execution
		VK_CHECK(vkQueueSubmit2(m_graphics_queue, 1, &submit, get_current_frame().render_fence));

		//prepare present
		// this will put the image we just rendered to into the visible window.
		// we want to wait on the _renderSemaphore for that, 
		// as its necessary that drawing commands have finished before the image is displayed to the user
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_swapchain;
		presentInfo.swapchainCount = 1;

		presentInfo.pWaitSemaphores = &get_current_frame().render_semaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &swapchain_image_index;

		VK_CHECK(vkQueuePresentKHR(m_graphics_queue, &presentInfo));

		//increase the number of frames drawn
		m_frame_number++;

	}

	void vk_renderer::refresh(f32 delta_time) { }

	void vk_renderer::set_size(u32 width, u32 height) { }

	f32 vk_renderer::get_aspect_ratio() {

		return 1.f;
	}

	void vk_renderer::wait_idle() { }

}
