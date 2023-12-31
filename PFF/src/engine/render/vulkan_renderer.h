#pragma once

#include "util/util.h"

namespace PFF{

	class vk_device;
	class vk_swapchain;
	class vk_pipeline;

	class vulkan_renderer {
	public:
		vulkan_renderer(std::shared_ptr<pff_window> window);
		~vulkan_renderer();

		vulkan_renderer(const vulkan_renderer&) = delete;
		vulkan_renderer& operator=(const vulkan_renderer&) = delete;

	private:

		void create_pipeline_layout();
		void create_pipeline();
		void create_command_buffer();
		void draw_frame();

		std::shared_ptr<pff_window> m_window;

		std::shared_ptr<vk_device> m_device;
		std::shared_ptr<vk_swapchain> m_swapchain;
		std::unique_ptr<vk_pipeline> m_vk_pipeline;

		VkPipelineLayout m_pipeline_layout;
		std::vector<VkCommandBuffer> m_command_buffers;
	};

}