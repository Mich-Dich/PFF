#pragma once

#include "engine/render/vk_device.h"
#include "engine/render/vk_swapchain.h"
#include "engine/render/render_system.h"
#include "engine/platform/pff_window.h"
#include "engine/layer/layer_stack.h"
#include "engine/map/game_map.h"
#include "engine/layer/world_layer.h"
#include "engine/render/vk_descripters.h"

namespace PFF {

	class renderer {
	public:

		renderer(std::shared_ptr<pff_window> window, layer_stack* layerstack);
		~renderer();

		DELETE_COPY(renderer);

		FORCEINLINE void set_world_Layer(world_layer* worldlayer) { m_world_Layer = worldlayer; }
		FORCEINLINE bool is_frame_started() const { return m_is_frame_started; }
		FORCEINLINE std::shared_ptr<vk_device> get_device() const { return m_device; }
		FORCEINLINE VkRenderPass get_swapchain_render_pass() const { return m_swapchain->get_render_pass(); }
		FORCEINLINE u32 get_swapchain_image_count() const { return m_swapchain->get_image_count(); }
		FORCEINLINE float get_aspect_ratio() const { return m_swapchain->get_extentAspectRatio(); }
		FORCEINLINE VkFramebuffer get_image_view() const { return m_swapchain->getFrameBuffer(m_current_image_index); }
		FORCEINLINE void set_state(system_state state) { CORE_LOG(Debug, "setting renderer state");  m_state = state; }
		FORCEINLINE u32 get_render_system_pipeline_subpass() const { return m_render_system->get_pipeline_subpass(); }
		FORCEINLINE VkDescriptorPool get_global_descriptor_pool() const { return m_global_descriptor_pool->get_descriptorPool(); }

		FORCEINLINE VkCommandBuffer get_current_command_buffer() const {	CORE_ASSERT(m_is_frame_started, "", "Cant get command buffer when frame not in progress");
																			return m_command_buffers[m_current_image_index]; }
		FORCEINLINE u32 get_frame_index() const {							CORE_ASSERT(m_is_frame_started, "", "Cant get command buffer when frame not in progress");
																			return m_current_image_index; }

		void add_render_system(std::shared_ptr<vk_device> device, VkRenderPass renderPass, VkDescriptorSetLayout descriptor_set_layout);
		void draw_frame(const f32 delta_time);
		void wait_Idle();
		void set_size(const u32 width, const u32 height);
		void refresh(const f32 delta_time);

	private:

		VkCommandBuffer begin_frame();
		void end_frame();
		void begin_swapchain_renderpass(VkCommandBuffer commandbuffer);
		void end_swapchain_renderpass(VkCommandBuffer commandbuffer);
		void create_command_buffer();
		void free_command_buffers();
		void recreate_swapchian();

		system_state m_state;
		bool needs_to_resize;
		u32 m_current_image_index;
		bool m_is_frame_started = false;

		world_layer* m_world_Layer;								// *application::get().get_world_layer();

		std::shared_ptr<pff_window> m_window;
		std::shared_ptr<vk_device> m_device;
		std::unique_ptr<vk_swapchain> m_swapchain{};
		std::unique_ptr<render_system> m_render_system{};
		layer_stack* m_layerstack;
		std::vector<game_object> m_game_objects{};				// move into map class

		std::vector<std::unique_ptr<vk_buffer>> m_global_UBO_buffer{};
		std::vector<VkCommandBuffer> m_command_buffers{};
		std::unique_ptr<vk_descriptor_pool> m_global_descriptor_pool;
		std::unique_ptr<vk_descriptor_set_layout> m_global_set_layout;
		std::vector<VkDescriptorSet> m_global_descriptor_set{};

	};

}
