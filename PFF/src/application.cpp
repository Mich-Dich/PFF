
#include "util/pffpch.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"

#include "engine/layer/layer.h"
#include "engine/layer/layer_stack.h"
#include "engine/layer/imgui_layer.h"
#include "engine/layer/world_layer.h"

#include "engine/platform/pff_window.h"
#include "engine/render/renderer.h"
#include "engine/game_objects/camera.h"
#include "util/timer.h"

#include "application.h"

namespace PFF {

	// ==================================================================== setup ====================================================================

	application* application::s_instance = nullptr;
	ref<PFF::render::renderer> application::m_renderer;
	ref<pff_window> application::m_window;
	//world_layer* application::m_world_layer;
	bool application::m_is_titlebar_hovered;
	bool application::m_running;


	application::application() {

		PFF_PROFILE_BEGIN_SESSION("startup", "benchmarks", "PFF_benchmark_startup.json");
		PFF_PROFILE_FUNCTION();

		PFF::Logger::Init("[$B$T:$J$E] [$B$L$X $I - $P:$G$E] $C$Z");
		CORE_ASSERT(!s_instance, "", "Application already exists");

		CORE_LOG_INIT();
		s_instance = this;

		// ---------------------------------------- general subsystems ----------------------------------------
		config::init();

		fps_timer = timer();
		fps_timer.set_fps_settings(m_target_fps);

		m_layerstack = create_ref<layer_stack>();

		m_window = std::make_shared<pff_window>();
		m_window->set_event_callback(BIND_FN(application::on_event));
		
		// ---------------------------------------- renderer ----------------------------------------
		m_renderer = create_ref<render::vulkan::vk_renderer>(m_window, m_layerstack);
		
		// ---------------------------------------- layers ----------------------------------------
		m_world_layer = new world_layer();
		m_layerstack->push_layer(m_world_layer);
		m_renderer->set_active_camera(m_world_layer->get_editor_camera());

		m_imgui_layer = new UI::imgui_layer();
		m_layerstack->push_overlay(m_imgui_layer);

		PFF_PROFILE_END_SESSION();
	}

	application::~application() {

		PFF_PROFILE_BEGIN_SESSION("shutdown", "benchmarks", "PFF_benchmark_shutdown.json");
		
		m_renderer->set_state(system_state::inactive);

		m_layerstack->pop_overlay(m_imgui_layer);
		delete m_imgui_layer;

		//m_current_map.reset();
		//m_layerstack.pop_layer(m_world_layer);
		//delete m_world_layer;

		m_renderer.reset();
		m_window.reset();
		m_layerstack.reset();

		CORE_LOG_SHUTDOWN();
		PFF_PROFILE_END_SESSION();
	}

	// ==================================================================== main loop ====================================================================

	void application::run() {

		PFF_PROFILE_BEGIN_SESSION("runtime", "benchmarks", "PFF_benchmark_runtime.json");

		client_init();

		while (m_running) {

			PFF_PROFILE_SCOPE("run");			
			m_window->poll_events();				// update internal state
			
			update(m_delta_time);					// client update
			for (layer* layer : *m_layerstack)		// engine update for all layers [world_layer, debug_layer, imgui_layer]
				layer->on_update(m_delta_time);

			render(m_delta_time);					// client render
			m_renderer->draw_frame(m_delta_time);	// engine render

			fps_timer.limit_fps(m_limit_fps, m_fps, m_delta_time, m_work_time, m_sleep_time);
		}

		m_renderer->wait_idle();
		
		CORE_ASSERT(shutdown(), "client application is shutdown", "client-defint shutdown() has failed");			// init user code / potentally make every actor have own function (like UNREAL)

		PFF_PROFILE_END_SESSION();
	}

	// ==================================================================== PUBLIC ====================================================================

	void application::limit_fps(const bool new_value, const u32 new_limit) {

		m_limit_fps = new_value;
		fps_timer.set_fps_settings(new_limit);
	}

	void application::get_fps_values(bool& limit_fps, u32& target_fps, u32& current_fps, f32& work_time, f32& sleep_time) {

		limit_fps = m_limit_fps;
		target_fps = (m_focus ? m_target_fps : m_nonefocus_fps);
		current_fps = m_fps;
		work_time = m_work_time * 1000;
		sleep_time = m_sleep_time;
	}
 
	void application::capture_cursor() { m_window->capture_cursor(); }

	void application::release_cursor() { m_window->release_cursor(); }

	void application::minimize_window() { m_window->queue_event([window = m_window] { window->minimize_window(); }); }

	void application::restore_window() { m_window->queue_event([window = m_window] { window->restore_window(); }); }

	void application::maximize_window() { m_window->queue_event([window = m_window] { window->maximize_window(); }); }

	// ==================================================================== PRIVATE ====================================================================
	
	void application::client_init() {

		// ---------------------------------------- client side ----------------------------------------
		CORE_ASSERT(init(), "client application is intalized", "client-defint init() has failed");			// init user code / potentally make every actor have own function (like UNREAL)

		// ---------------------------------------- finished setup ----------------------------------------
		m_renderer->set_state(system_state::active);
		m_is_titlebar_hovered = false;
		m_running = true;

		m_window->show_window(true);
		m_window->poll_events();
		fps_timer.start_measurement();
	}

	void application::client_shutdown() {

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
			for (auto layer = m_layerstack->end(); layer != m_layerstack->begin(); ) {

				(*--layer)->on_event(event);
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
		//m_world_layer->get_editor_camera()->set_aspect_ratio(m_renderer->get_aspect_ratio());

		return true;
	}

	bool application::on_window_refresh(window_refresh_event& event) {

		PFF_PROFILE_FUNCTION();

		fps_timer.limit_fps(false, m_fps, m_delta_time, m_work_time, m_sleep_time);
		// m_imgui_layer->set_fps_values(m_limit_fps, (m_focus ? m_target_fps : m_nonefocus_fps), m_fps, static_cast<f32>(m_work_time * 1000), static_cast<f32>(m_sleep_time * 1000));
		m_renderer->refresh(m_delta_time);
		return true;
	}

	bool PFF::application::on_window_focus(window_focus_event& event) {

		PFF_PROFILE_FUNCTION();

		m_focus = event.get_focus();
		if (event.get_focus())
			fps_timer.set_fps_settings(m_target_fps);
		else
			fps_timer.set_fps_settings(m_nonefocus_fps);

		return true;
	}

}
