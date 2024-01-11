#pragma once

#include "engine/render/vk_device.h"
#include "engine/render/vk_swapchain.h"
#include "engine/render/render_system.h"
#include "engine/platform/pff_window.h"
#include "engine/layer/layer_stack.h"
#include "engine/map/game_map.h"

namespace PFF {

	class renderer {
	public:

		renderer(std::shared_ptr<pff_window> window, layer_stack* layerstack);
		~renderer();

		DELETE_COPY(renderer);

		FORCEINLINE bool is_frame_started() const { return m_is_frame_started; }
		FORCEINLINE std::shared_ptr<vk_device> get_device() const { return m_device; }
		FORCEINLINE VkDescriptorPool get_imgui_descriptor_pool() const { return m_imgui_descriptor_pool; }
		FORCEINLINE VkRenderPass get_swapchain_render_pass() const { return m_swapchain->get_render_pass(); }
		FORCEINLINE u32 get_swapchain_image_count() const { return m_swapchain->get_image_count(); }
		FORCEINLINE u32 get_render_system_pipeline_subpass() const { return m_render_system->get_pipeline_subpass(); }
		FORCEINLINE VkCommandBuffer get_current_command_buffer() const {	CORE_ASSERT(m_is_frame_started, "", "Cant get command buffer when frame not in progress");
																			return m_command_buffers[m_current_image_index]; }
		FORCENOINLINE void set_current_map(std::shared_ptr<game_map> current_map) { m_current_map = current_map; }

		void add_render_system(std::shared_ptr<vk_device> device, VkRenderPass renderPass);
		void draw_frame(f32 delta_time);
		void wait_Idle();
		void set_size(u32 width, u32 height);
		void refresh(f32 delta_time);

	private:

		VkCommandBuffer begin_frame();
		void end_frame();
		void begin_swapchain_renderpass(VkCommandBuffer commandbuffer);
		void end_swapchain_renderpass(VkCommandBuffer commandbuffer);
		void create_command_buffer();
		void free_command_buffers();
		void recreate_swapchian();

		std::shared_ptr<game_map> m_current_map{};
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
