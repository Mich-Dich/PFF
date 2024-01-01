#pragma once

#include "util/util.h"

namespace PFF{

	class vk_device;
	class vk_swapchain;
	class vk_pipeline;
	class basic_mesh;

	class vulkan_renderer {
	public:
		vulkan_renderer(std::shared_ptr<pff_window> window);
		~vulkan_renderer();

		void draw_frame();
		void wait_Idle();
		void set_size(u32 width, u32 height);

		DELETE_COPY(vulkan_renderer);

	private:
		void load_meshes();
		void create_pipeline_layout();
		void create_pipeline();
		void create_command_buffer();
		void recreate_swapchian();
		void recordCommandBuffer(int32 image_index);

		std::unique_ptr<basic_mesh> m_testmodel;

		std::shared_ptr<pff_window> m_window;
		std::shared_ptr<vk_device> m_device;
		std::shared_ptr<vk_swapchain> m_swapchain;
		std::unique_ptr<vk_pipeline> m_vk_pipeline;

		VkPipelineLayout m_pipeline_layout;
		std::vector<VkCommandBuffer> m_command_buffers;
	};

}