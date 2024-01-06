#pragma once

// DEV-ONLY
#include "engine/render/vk_pipeline.h"

namespace PFF {

	class game_object;
	class renderer;
	class vk_device;

	class render_system {
	public:

		render_system(std::shared_ptr<vk_device> device, VkRenderPass renderPass);
		~render_system();
		
		DELETE_COPY(render_system);

		void render_game_objects(VkCommandBuffer command_buffer, std::vector<game_object>& game_objects);

	private:

		void create_pipeline_layout();
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