
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"

#include "engine/platform/pff_window.h"
#include "engine/render/vulkan_renderer.h"

#include "application.h"


namespace PFF {

	application::application() :
		m_delta_time(1), m_running(true) {

		CORE_LOG(Debug, "application Constructor");
		CORE_ASSERT(!s_instance, "", "Application already exists");
		s_instance = this;

		// TODO: load WindowAttributes from config file
		WindowAttributes loc_window_att = WindowAttributes();
		m_window = std::make_shared<pff_window>(loc_window_att);			// Can be called after inital setup like [compiling shaders]
		m_window->SetEventCallback(STD_BIND_EVENT_FN(application::on_event));

		m_vulkan_renderer = std::make_shared<vulkan_renderer>(m_window);
	}

	application::~application() {

		LOG(Info, "Destroying vk_pipeline");
	}

	void application::on_event(event& event) {

		event_dispatcher dispatcher(event);
		dispatcher.dispatch<window_close_event>([&](window_close_event& event) {

				m_running = false;
				return true;
			});

		dispatcher.dispatch<window_resize_event>([&](window_resize_event& event) {
			
			m_vulkan_renderer->set_size(event.get_width(), event.get_height());
			return true;
			});

	}

	void application::run() {
	
		initalize();

		CORE_LOG(Trace, "Running")

			while (m_running) {
				glfwPollEvents();

				update(m_delta_time);	// potentall remove  (like UNREAL)
				render(m_delta_time);	// potentall remove  (like UNREAL)
				//m_vulkan_renderer->draw_frame();
			}

		m_vulkan_renderer->wait_Idle();
	}

	bool application::initalize() {
		return true;
	}

	bool application::update(f32 delta_time) {
		return true;
	}

	bool application::render(f32 delta_time) {
		return true;
	}

	void application::on_window_resize(event& event) {
	}

}
