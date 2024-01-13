
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

		m_world_layer = new world_layer();
		m_layerstack.push_layer(m_world_layer);

		m_imgui_layer = new imgui_layer(m_renderer);
		m_layerstack.push_overlay(m_imgui_layer);

		init();					// init user code / potentally make every actor have own function (like UNREAL)
	}

	application::~application() {
		
		shutdown();
		
		m_layerstack.pop_overlay(m_imgui_layer);
		m_layerstack.pop_layer(m_world_layer);

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

		std::shared_ptr<basic_mesh> model = createCubeModel(m_renderer->get_device(), { 0.0f, 0.0f, 0.0f });
		auto cube = m_world_layer->get_current_map()->create_empty_game_object();
		cube->mesh = model;
		cube->transform.translation = { .0f, .0f, .5f };
		cube->transform.scale = { .5f, .5f, .5f };


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

	std::unique_ptr<basic_mesh> application::createCubeModel(std::shared_ptr<vk_device> device, glm::vec3 offset) {

		std::vector<basic_mesh::vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<basic_mesh>(device, vertices);
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
