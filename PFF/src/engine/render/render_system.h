#pragma once

// DEV-ONLY
#include "engine/render/vk_pipeline.h"

namespace PFF {

	class game_object;
	class renderer;
	class vk_device;

	class render_system {
	public:

		render_system(std::shared_ptr<vk_device> device, VkRenderPass renderPass, VkDescriptorSetLayout descriptor_set_layout);
		~render_system();
		
		DELETE_COPY(render_system);

		FORCEINLINE u32 get_pipeline_subpass() const { return m_vk_pipeline->get_subpass(); }
		FORCEINLINE void pipeline_bind_commnad_buffers(VkCommandBuffer command_buffer) const { return m_vk_pipeline->bind_commnad_buffers(command_buffer); }
		//void bind_commnad_buffers(VkCommandBuffer command_buffer);

		void render_game_objects(f32 delta_time, VkCommandBuffer command_buffer, std::vector<game_object>& game_objects);

	private:

		void create_pipeline_layout(VkDescriptorSetLayout descriptor_set_layout);
		void create_pipeline(VkRenderPass renderPass);

		bool m_active;
		bool needs_to_resize;

		std::shared_ptr<vk_device> m_device;
		std::shared_ptr<renderer> m_renderer;

		std::unique_ptr<vk_pipeline> m_vk_pipeline;
		VkPipelineLayout m_pipeline_layout;
		pipeline_config_info m_pipeline_config;
	};

}