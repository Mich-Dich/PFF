#pragma once

#include "engine/render/renderer.h"
#include "engine/platform/pff_window.h"

namespace PFF::render::vulkan {

	// framedata	// TODO: extract into own file
	struct FrameData {

		VkSemaphore swapchain_semaphore, render_semaphore;
		VkFence render_fence;

		VkCommandPool command_pool;
		VkCommandBuffer main_command_buffer;
	};

	constexpr u32 FRAME_COUNT = 2;

	class vk_renderer : public PFF::renderer {
	public:

		vk_renderer(ref<pff_window> window);
		~vk_renderer();

		void draw_frame(f32 delta_time) override;
		void refresh(f32 delta_time) override;
		void set_size(u32 width, u32 height) override;
		f32 get_aspect_ratio() override;
		void wait_idle() override;

	private:

		void create_swapchain(u32 width, u32 height);
		void destroy_swapchain();
		void init_commands();
		void init_sync_structures();

		bool m_is_initialized = false;
		int m_frame_number = 0;
		ref<pff_window> m_window;

		FORCEINLINE FrameData& get_current_frame() { return m_frames[m_frame_number % FRAME_COUNT]; };

		// ---------------------------- instance ---------------------------- 
		VkInstance					m_instance;			// Vulkan library handle
		VkDebugUtilsMessengerEXT	m_debug_messenger;	// Vulkan debug output handle
		VkPhysicalDevice			m_chosenGPU;		// GPU chosen as the default device
		VkDevice					m_device;			// Vulkan device for commands
		VkSurfaceKHR				m_surface;			// Vulkan window surface
		
		// ---------------------------- queues ---------------------------- 
		FrameData					m_frames[FRAME_COUNT];
		VkQueue						m_graphics_queue;
		u32							m_graphics_queue_family;
		
		// ---------------------------- swapchain ---------------------------- 
		VkSwapchainKHR				m_swapchain {};
		VkFormat					m_swapchain_image_format {};
		std::vector<VkImage>		m_swapchain_images {};
		std::vector<VkImageView>	m_swapchain_image_views {};
		VkExtent2D					m_swapchain_extent {};

	};
}
