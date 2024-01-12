
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

#include "util/timer.h"

#include "application.h"

namespace PFF {

	// ==================================================================== setup ====================================================================

	application* application::s_instance = nullptr;


	application::application() :
		m_delta_time(1), m_running(true) {

		CORE_LOG(Debug, "application Constructor");
		CORE_ASSERT(!s_instance, "", "Application already exists");
		s_instance = this;

		config::init();

		// init FPS system
		fps_timer = std::make_unique<timer>();
		fps_timer->set_fps_settings(m_target_fps);

		window_attributes loc_window_att = window_attributes();
		LOAD_CONFIG_STR(default_editor, loc_window_att.title, "window_attributes", "title");
		LOAD_CONFIG_NUM(default_editor, loc_window_att.width, u32, "window_attributes", "width");
		LOAD_CONFIG_NUM(default_editor, loc_window_att.height, u32, "window_attributes", "height");
		LOAD_CONFIG_BOOL(default_editor, loc_window_att.vsync, "window_attributes", "vsync");
		m_window = std::make_shared<pff_window>(loc_window_att);			// Can be called after inital setup like [compiling shaders]

		m_renderer = std::make_shared<renderer>(m_window, &m_layerstack);
		m_window->set_event_callback(BIND_FN(application::on_event));

		m_imgui_layer = new imgui_layer(m_renderer);
		m_layerstack.push_overlay(m_imgui_layer);

		m_current_map = std::make_shared<game_map>();
		m_renderer->set_current_map(m_current_map);

		init();
	}

	application::~application() {
		
		shutdown();
		m_layerstack.pop_overlay(m_imgui_layer);
		m_renderer.reset();

		window_attributes loc_window_att = m_window->get_attributes();
		SAVE_CONFIG_STR(default_editor, loc_window_att.title, "window_attributes", "title");
		SAVE_CONFIG_NUM(default_editor, loc_window_att.width, u32, "window_attributes", "width");
		SAVE_CONFIG_NUM(default_editor, loc_window_att.height, u32, "window_attributes", "height");
		SAVE_CONFIG_BOOL(default_editor, loc_window_att.vsync, "window_attributes", "vsync");
		m_window.reset();

	}

	// ==================================================================== main loop ====================================================================

	void application::run() {

		init();							// init user code / potentally make every actor have own function (like UNREAL)

		//m_renderer->create_dummy_game_objects();		// TODO: move object creation to map

		std::vector<basic_mesh::vertex> vertices;
		vertices = {
			{{0.0f,-0.5f}},
			{{0.5f,0.5f}},
			{{-0.5f,0.5f}},
		};
		auto model = std::make_shared<basic_mesh>(m_renderer->get_device(), vertices);

		auto triangle = m_current_map->create_empty_game_object();
		triangle->mesh = model;
		triangle->color = { .02f, 1.0f, .02f };
		triangle->transform_2D.translation.x = .2f;
		triangle->transform_2D.scale = { 2.0f ,0.5f };
		triangle->transform_2D.rotation_speed = 0.05f;
		triangle->transform_2D.rotation = 0.25f * two_pi<float>();


		while (m_running) {

			m_window->update();

			// update all layers
			for (layer* layer : m_layerstack) 
				layer->on_update();			

			// TODO: MOVE to world_layer
			for (auto& obj : m_current_map->get_all_game_objects())
				obj.transform_2D.rotation = obj.transform_2D.rot(m_delta_time);


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

		event_dispatcher dispatcher(event);
		dispatcher.dispatch<window_close_event>(BIND_FN(application::on_window_close));
		dispatcher.dispatch<window_resize_event>(BIND_FN(application::on_window_resize));
		dispatcher.dispatch<window_refresh_event>(BIND_FN(application::on_window_refresh));
		dispatcher.dispatch<window_focus_event>(BIND_FN(application::on_window_focus));

		for (auto it = m_layerstack.end(); it != m_layerstack.begin(); ) {
			(*--it)->on_event(event);
			if (event.handled)
				break;
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

		return false;
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
