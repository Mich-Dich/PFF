
// vulkan headers
#include <vulkan/vulkan.h>

#include "util/util.h"

#include "engine/platform/pff_window.h"
#include "engine/render/vk_device.h"
#include "engine/render/vk_pipeline.h"
#include "engine/render/vk_swap_chain.h"

#include "vulkan_renderer.h"


namespace PFF {

	vulkan_renderer::vulkan_renderer(std::shared_ptr<pff_window> window) {
	
		m_device = std::make_shared<vk_device>(m_window);
		m_vk_pipeline = std::make_shared<vk_pipeline>(m_device, pipeline_cinfig_info{}, "./shaders/default.frag.spv", "./shaders/default.vert.spv");
		m_swap_chain = std::make_shared<vk_swap_chain>(m_device, m_window->get_extend());
	}

	vulkan_renderer::~vulkan_renderer() {


	}

}
