#pragma once

#include "engine/render/vk_device.h"
#include "engine/render/vk_swapchain.h"
#include "engine/render/render_system.h"
#include "engine/platform/pff_window.h"
#include "engine/layer/layer_stack.h"

namespace PFF {

	class renderer {
	public:

		renderer(std::shared_ptr<pff_window> window, layer_stack* layerstack);
		~renderer();

		DELETE_COPY(renderer);

		FORCEINLINE bool is_frame_started() const { return m_is_frame_started; }
		FORCEINLINE std::shared_ptr<vk_device> get_device() const { return m_device; }
		FORCEINLINE VkRenderPass get_swapchain_render_pass() const { return m_swapchain->get_render_pass(); }
		FORCEINLINE VkCommandBuffer get_current_command_buffer() const {	CORE_ASSERT(m_is_frame_started, "", "Cant get command buffer when frame not in progress");
																			return m_command_buffers[m_current_image_index]; }

		void add_render_system(std::shared_ptr<vk_device> device, VkRenderPass renderPass);
		void draw_frame();
		void wait_Idle();
		void set_size(u32 width, u32 height);
		void refresh();
		void create_dummy_game_objects();

	private:

		void imgui_init();
		void imgui_sutdown();
		void imgui_begin_frame();
		void imgui_end_frame(VkCommandBuffer commandbuffer);

		VkCommandBuffer begin_frame();
		void end_frame();
		void begin_swapchain_renderpass(VkCommandBuffer commandbuffer);
		void end_swapchain_renderpass(VkCommandBuffer commandbuffer);
		void create_command_buffer();
		void free_command_buffers();
		void recreate_swapchian();
		
		system_state state;
		bool needs_to_resize;
		u32 m_current_image_index;
		bool m_is_frame_started = false;

		std::shared_ptr<pff_window> m_window;
		std::shared_ptr<vk_device> m_device;
		std::unique_ptr<render_system> m_render_system{};
		layer_stack* m_layerstack;

		std::vector<game_object> m_game_objects{};				// move into map class
		std::unique_ptr<vk_swapchain> m_swapchain{};
		std::vector<VkCommandBuffer> m_command_buffers{};
		VkDescriptorPool m_imgui_descriptor_pool{};
	};

}
