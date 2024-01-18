
#include "util/pffpch.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"

#include "engine/layer/layer_stack.h"
#include "engine/layer/layer.h"
#include "engine/layer/imgui_layer.h"

#include "engine/platform/pff_window.h"
#include "engine/render/renderer.h"
#include "engine/map/game_map.h"

// DEV-ONLY:
#include "engine/game_objects/camera.h"


#include "util/timer.h"

#include "application.h"

namespace PFF {

	// ==================================================================== setup ====================================================================

	application* application::s_instance = nullptr;


	application::application() :
		m_delta_time(1), m_running(true) {

		PFF::Logger::Init("[$B$T:$J$E] [$B$L$X $K - $P:$G$E] $C$Z");
		CORE_LOG(Debug, "application Constructor");
		CORE_ASSERT(!s_instance, "", "Application already exists");
		s_instance = this;

		config::init();

		// init FPS system
		fps_timer = std::make_unique<timer>();
		fps_timer->set_fps_settings(m_target_fps);

		m_window = std::make_shared<pff_window>();			// Can be called after inital setup like [compiling shaders]
		m_renderer = std::make_shared<renderer>(m_window, &m_layerstack);
		m_window->set_event_callback(BIND_FN(application::on_event));

		m_world_layer = new world_layer();
		m_layerstack.push_layer(m_world_layer);

		m_imgui_layer = new imgui_layer(m_renderer);
		m_layerstack.push_overlay(m_imgui_layer);

		m_renderer->set_state(system_state::active);
		init();					// init user code / potentally make every actor have own function (like UNREAL)
	}

	application::~application() {

		shutdown();

		m_current_map.reset();

		m_layerstack.pop_overlay(m_imgui_layer);
		delete m_imgui_layer;

		m_layerstack.pop_layer(m_world_layer);
		delete m_world_layer;

		m_renderer.reset();
		m_window.reset();
		fps_timer.reset();
		CORE_LOG(Info, "Shutdown");
	}

	// ==================================================================== main loop ====================================================================

	void application::capture_cursor() { m_window->capture_cursor(); }

	void application::release_cursor() { m_window->release_cursor(); }


	void application::run() {

		while (m_running) {

			m_window->update();

			// update all layers
			for (layer* layer : m_layerstack)
				layer->on_update(m_delta_time);

			update(m_delta_time);	// potentally make every actor have own function (like UNREAL)
			render(m_delta_time);	// potentally make every actor have own function (like UNREAL)

			m_imgui_layer->set_fps_values((m_focus ? m_target_fps : m_nonefocus_fps), m_fps, static_cast<f32>(m_work_time * 1000), static_cast<f32>(m_sleep_time * 1000));
			m_renderer->draw_frame(m_delta_time);

			// Simple FPS controller - needs work
			fps_timer->limit_fps(m_fps, m_delta_time, m_work_time, m_sleep_time);
		}

		m_renderer->wait_Idle();
	}

	// ==================================================================== event handling ====================================================================

	void application::on_event(event& event) {

		// application events
		event_dispatcher dispatcher(event);
		dispatcher.dispatch<window_close_event>(BIND_FN(application::on_window_close));
		dispatcher.dispatch<window_resize_event>(BIND_FN(application::on_window_resize));
		dispatcher.dispatch<window_refresh_event>(BIND_FN(application::on_window_refresh));
		dispatcher.dispatch<window_focus_event>(BIND_FN(application::on_window_focus));

		// none application events
		if (!event.handled) {

			for (auto it = m_layerstack.end(); it != m_layerstack.begin(); ) {

				(*--it)->on_event(event);
				if (event.handled)
					break;
			}
		}
	}

	bool application::on_window_close(window_close_event& event) {

		m_running = false;
		return true;
	}

	bool application::on_window_resize(window_resize_event& event) {

		m_renderer->set_size(event.get_width(), event.get_height());
		return true;
	}

	bool application::on_window_refresh(window_refresh_event& event) {
		
		fps_timer->end_measurement(m_fps, m_delta_time, m_work_time, m_sleep_time);
		m_imgui_layer->set_fps_values((m_focus ? m_target_fps : m_nonefocus_fps), m_fps, static_cast<f32>(m_work_time * 1000), static_cast<f32>(m_sleep_time * 1000));
		m_renderer->refresh(m_delta_time);
		fps_timer->start_measurement();
		return true;
	}

	bool PFF::application::on_window_focus(window_focus_event& event) {

		m_focus = event.get_focus();
		if (event.get_focus())
			fps_timer->set_fps_settings(m_target_fps);
		else
			fps_timer->set_fps_settings(m_nonefocus_fps);

		return true;
	}


	// ==================================================================== engine events ====================================================================

	bool application::init() {
		return true;
	}

	bool application::update(f32 delta_time) {
		return true;
	}

	bool application::render(f32 delta_time) {
		return true;
	}

	bool application::shutdown() {
		return true;
	}

}
