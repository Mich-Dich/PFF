#pragma once

// DEV-ONLY
#include "engine/render/vk_pipeline.h"
#include "engine/game_objects/camera.h"

#include "engine/render/frame_info.h"

namespace PFF {

	class game_object;
	class renderer;
	class vk_device;

	class mesh_render_system {
	public:

		mesh_render_system(ref<vk_device> device, VkRenderPass renderPass, VkDescriptorSetLayout descriptor_set_layout);
		~mesh_render_system();
		
		DELETE_COPY(mesh_render_system);

		FORCEINLINE u32 get_pipeline_subpass() const { return m_vk_pipeline->get_subpass(); }
		FORCEINLINE void pipeline_bind_commnad_buffers(VkCommandBuffer command_buffer) const { return m_vk_pipeline->bind_commnad_buffers(command_buffer); }
		//void bind_commnad_buffers(VkCommandBuffer command_buffer);

		virtual void render(frame_info frame_info);
		void render_game_objects(frame_info frame_info);

	private:

		void create_pipeline_layout(VkDescriptorSetLayout descriptor_set_layout);
		void create_pipeline(VkRenderPass renderPass);

		bool m_active;
		bool needs_to_resize;

		ref<vk_device> m_device;
		ref<renderer> m_renderer;

		scope_ref<vk_pipeline> m_vk_pipeline;
		VkPipelineLayout m_pipeline_layout;
		pipeline_config_info m_pipeline_config;
	};

}