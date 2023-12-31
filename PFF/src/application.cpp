
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "engine/platform/pff_window.h"
#include "engine/render/vulkan_renderer.h"

#include "application.h"


namespace PFF {

	application::application() {

		CORE_LOG(Debug, "application Constructor");
		CORE_ASSERT(!s_instance, "", "Application already exists");
		s_instance = this;

		// TODO: load WindowAttributes from config file and pass into constructor
		m_window = std::make_shared<pff_window>("test", 800, 600);			// Can be called after inital setup like [compiling shaders]
		m_vulkan_renderer = std::make_shared<vulkan_renderer>(m_window);
	}

	application::~application() {

		LOG(Info, "Destroying vk_pipeline");
	}

	void application::run() {
	
		CORE_LOG(Trace, "Running")

			while (!m_window->should_close()) {
				glfwPollEvents();
			}
	}
	/*
	void application::on_event() {}

	bool application::initalize() {
		return true;
	}

	bool application::update(f64 delta_time) {
		return true;
	}

	bool application::render(f64 delta_time) {
		return true;
	}*/

}
