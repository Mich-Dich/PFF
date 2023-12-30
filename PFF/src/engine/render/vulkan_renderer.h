#pragma once

#include "util/util.h"

namespace PFF{

	class vk_swap_chain;

	class vulkan_renderer {
	public:
		vulkan_renderer(std::shared_ptr<pff_window> window);
		~vulkan_renderer();

	private:

		std::shared_ptr<pff_window> m_window;

		std::shared_ptr<vk_device> m_device;
		std::shared_ptr<vk_pipeline> m_vk_pipeline;
		std::shared_ptr<vk_swap_chain> m_swap_chain;
	};

}