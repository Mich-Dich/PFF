#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>


#include "application.h"
#include "util/util.h"
#include "engine/platform/pff_window.h"


namespace PFF {

	application::application() {

		CORE_LOG(Debug, "application Constructor");
		CORE_ASSERT(!s_instance, "", "Application already exists");
		s_instance = this;

		// TODO: load WindowAttributes from config file and pass into constructor
		m_window = new pff_window("test", 800, 600);			// Can be called after inital setup like [compiling shaders]
	}

	application::~application() {}

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

