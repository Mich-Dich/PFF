
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
#include "engine/render/vk_buffer.h"
#include "engine/map/game_map.h"

// DEV-ONLY:
#include "engine/game_objects/camera.h"


#include "util/timer.h"

#include "application.h"

namespace PFF {

	// ==================================================================== setup ====================================================================

	application* application::s_instance = nullptr;


	application::application() 
		: m_delta_time(1), m_running(true) {

		PFF_PROFILE_BEGIN_SESSION("startup", "benchmarks", "PFF_benchmark_startup.json");
		PFF_PROFILE_FUNCTION();

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

		m_renderer->set_world_Layer(m_world_layer);
		m_renderer->set_state(system_state::active);

		init();					// init user code / potentally make every actor have own function (like UNREAL)
		PFF_PROFILE_END_SESSION();
	}

	application::~application() {

		PFF_PROFILE_BEGIN_SESSION("shutdown", "benchmarks", "PFF_benchmark_shutdown.json");
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
		PFF_PROFILE_END_SESSION();
	}

	// ==================================================================== main loop ====================================================================

	void application::capture_cursor() { m_window->capture_cursor(); }

	void application::release_cursor() { m_window->release_cursor(); }


	void application::run() {

		PFF_PROFILE_BEGIN_SESSION("runtime", "benchmarks", "PFF_benchmark_runtime.json");

		while (m_running) {

			m_window->update();

			// update all layers
			for (layer* layer : m_layerstack)
				layer->on_update(m_delta_time);

			update(m_delta_time);	// potentally make every actor have own function (like UNREAL)
			render(m_delta_time);	// potentally make every actor have own function (like UNREAL)

			m_imgui_layer->set_fps_values(m_limit_fps, (m_focus ? m_target_fps : m_nonefocus_fps), m_fps, static_cast<f32>(m_work_time * 1000), static_cast<f32>(m_sleep_time * 1000));
			m_renderer->draw_frame(m_delta_time);

			// Simple FPS controller - needs work
			fps_timer->limit_fps(m_limit_fps, m_fps, m_delta_time, m_work_time, m_sleep_time);
		}

		m_renderer->wait_Idle();
		PFF_PROFILE_END_SESSION();
	}

	// ==================================================================== event handling ====================================================================

	void application::on_event(event& event) {

		PFF_PROFILE_FUNCTION();

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

		PFF_PROFILE_FUNCTION();

		m_renderer->set_size(event.get_width(), event.get_height());

		// m_camera.set_orthographic_projection(-aspect, aspect, -1, 1, 0, 10);
		// m_camera.set_view_YXZ(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(0.0f));
		// m_camera.set_view_direction(glm::vec3{ 0.0f }, glm::vec3{ 0.5f, 0.0f, 1.0f });
		m_world_layer->get_editor_camera()->set_aspect_ratio(m_renderer->get_aspect_ratio());

		return true;
	}

	bool application::on_window_refresh(window_refresh_event& event) {

		PFF_PROFILE_FUNCTION();

		fps_timer->limit_fps(false, m_fps, m_delta_time, m_work_time, m_sleep_time);
		m_imgui_layer->set_fps_values(m_limit_fps, (m_focus ? m_target_fps : m_nonefocus_fps), m_fps, static_cast<f32>(m_work_time * 1000), static_cast<f32>(m_sleep_time * 1000));
		m_renderer->refresh(m_delta_time);
		fps_timer->start_measurement();
		return true;
	}

	bool PFF::application::on_window_focus(window_focus_event& event) {

		PFF_PROFILE_FUNCTION();

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
