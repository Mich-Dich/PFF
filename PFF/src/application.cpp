
#include "util/pffpch.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"

#include "engine/layer/layer_stack.h"
#include "engine/layer/layer.h"

#include "engine/platform/pff_window.h"
#include "engine/render/vulkan_renderer.h"

#include "engine/map/game_map.h"

#include "application.h"

namespace PFF {

	// ==================================================================== setup ====================================================================

	application::application() :
		m_delta_time(1), m_running(true), m_frame_start(std::chrono::system_clock::now()), m_frame_end(std::chrono::system_clock::now()) {

		CORE_LOG(Debug, "application Constructor");
		CORE_ASSERT(!s_instance, "", "Application already exists");
		s_instance = this;

		config::init();

		WindowAttributes loc_window_att = WindowAttributes();
		LOAD_CONFIG_STR(default_editor, loc_window_att.title, "WindowAttributes", "title");
		LOAD_CONFIG_NUM(default_editor, loc_window_att.width, u32, "WindowAttributes", "width");
		LOAD_CONFIG_NUM(default_editor, loc_window_att.height, u32, "WindowAttributes", "height");
		LOAD_CONFIG_BOOL(default_editor, loc_window_att.VSync, "WindowAttributes", "VSync");
		m_window = std::make_shared<pff_window>(loc_window_att);			// Can be called after inital setup like [compiling shaders]
		m_window->SetEventCallback(BIND_FN(application::on_event));

		m_vulkan_renderer = std::make_shared<vulkan_renderer>(m_window);
	}

	application::~application() {

		m_vulkan_renderer.reset();

		WindowAttributes loc_window_att = m_window->get_attributes();
		SAVE_CONFIG_STR(default_editor, loc_window_att.title, "WindowAttributes", "title");
		SAVE_CONFIG_NUM(default_editor, loc_window_att.width, u32, "WindowAttributes", "width");
		SAVE_CONFIG_NUM(default_editor, loc_window_att.height, u32, "WindowAttributes", "height");
		SAVE_CONFIG_BOOL(default_editor, loc_window_att.VSync, "WindowAttributes", "VSync");
		m_window.reset();

	}

	// ==================================================================== main loop ====================================================================

	void application::run() {

		init();
		m_targetdelta_time = (1000.0f / m_target_fps);

		CORE_LOG(Trace, "Running")

			while (m_running) {
				glfwPollEvents();

				// update all layers
				for (layer* layer : m_layerstack) {
					layer->on_update();
				}

				update(m_delta_time);	// potentally make private - every actor has own function (like UNREAL)
				render(m_delta_time);	// potentally make private - every actor has own function (like UNREAL)
				m_vulkan_renderer->draw_frame();



				// Simple FPS controller - needs work
				m_frame_end = std::chrono::system_clock::now();
				std::chrono::duration<double, std::milli> work_time = m_frame_end - m_frame_start;
				std::chrono::duration<double, std::milli> delta_ms(m_targetdelta_time - work_time.count());
				auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
				if (work_time.count() < m_targetdelta_time)
					std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));

				m_frame_start = std::chrono::system_clock::now();
				m_delta_time = std::chrono::duration<float, std::milli>(work_time + delta_ms_duration).count();
				//CORE_LOG(Trace, "FPS: " << std::fixed << std::setprecision(2) << 1000 / m_delta_time << " possible FPS: " << std::fixed << std::setprecision(2) << 1000 / work_time.count());
			}

		m_vulkan_renderer->wait_Idle();
	}

	// ==================================================================== event handling ====================================================================

	void application::on_event(event& event) {

		event_dispatcher dispatcher(event);
		dispatcher.dispatch<window_close_event>(BIND_FN(application::on_window_close));
		dispatcher.dispatch<window_resize_event>(BIND_FN(application::on_window_resize));
		dispatcher.dispatch<window_refresh_event>(BIND_FN(application::on_window_refresh));

		for (auto it = m_layerstack.end(); it != m_layerstack.begin(); ) {
			(*--it)->on_event(event);
			if (event.handled)
				break;
		}
	}

	bool application::on_window_resize(window_resize_event& event) {

		m_vulkan_renderer->set_size(event.get_width(), event.get_height());
		return true;
	}

	bool application::on_window_close(window_close_event& e) {

		m_running = false;
		return true;
	}

	bool application::on_window_refresh(window_refresh_event& e) {

		m_vulkan_renderer->refresh();
		return true;
	}

	// ==================================================================== layer system ====================================================================

	void application::push_layer(layer* layer) {

		m_layerstack.push_layer(layer);
	}

	void application::push_overlay(layer* overlay) {

		m_layerstack.push_overlay(overlay);
	}

	// ==================================================================== event handling ====================================================================

	bool application::init() {

		return true;
	}

	bool application::update(f32 delta_time) {

		return true;
	}

	bool application::render(f32 delta_time) {
		return true;
	}


}
