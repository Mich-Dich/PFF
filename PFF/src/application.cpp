
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

#include "application.h"

namespace PFF {

	// ==================================================================== setup ====================================================================

	application* application::s_instance = nullptr;


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

		m_renderer = std::make_shared<renderer>(m_window, &m_layerstack);
		m_window->SetEventCallback(BIND_FN(application::on_event));

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

		WindowAttributes loc_window_att = m_window->get_attributes();
		SAVE_CONFIG_STR(default_editor, loc_window_att.title, "WindowAttributes", "title");
		SAVE_CONFIG_NUM(default_editor, loc_window_att.width, u32, "WindowAttributes", "width");
		SAVE_CONFIG_NUM(default_editor, loc_window_att.height, u32, "WindowAttributes", "height");
		SAVE_CONFIG_BOOL(default_editor, loc_window_att.VSync, "WindowAttributes", "VSync");
		m_window.reset();

	}

	// ==================================================================== main loop ====================================================================

	void application::run() {

		init();							// init user code / potentally make every actor have own function (like UNREAL)
		m_targetdelta_time = (1000.0f / m_target_fps);

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
		triangle->color = { .1f, .8f, .1f };
		triangle->transform_2D.translation.x = .2f;
		//triangle->transform_2D.scale = { 2.0f ,0.5f };
		triangle->transform_2D.rotation_speed = 0.002f;
		//triangle->transform_2D.rotation = 0.25f * two_pi<float>();

		CORE_LOG(Trace, "Running")

			while (m_running) {
				glfwPollEvents();

				// update all layers
				for (layer* layer : m_layerstack) {
					layer->on_update();
				}

				update(m_delta_time);	// potentally make every actor have own function (like UNREAL)
				render(m_delta_time);	// potentally make every actor have own function (like UNREAL)

				m_renderer->draw_frame(m_delta_time);


				// Simple FPS controller - needs work
				m_frame_end = std::chrono::system_clock::now();
				std::chrono::duration<double, std::milli> work_time = m_frame_end - m_frame_start;
				std::chrono::duration<double, std::milli> delta_ms(m_targetdelta_time - work_time.count());
				auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
				if (work_time.count() < m_targetdelta_time)
					std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));

				m_frame_start = std::chrono::system_clock::now();
				m_delta_time = std::chrono::duration<float, std::milli>(work_time + delta_ms_duration).count();

				m_imgui_layer->set_fps_values(1000 / m_targetdelta_time, 1000 / m_delta_time, 1000 / work_time.count(), work_time.count());
				//CORE_LOG(Trace, "FPS: " << std::fixed << std::setprecision(2) << 1000 / m_delta_time << " possible FPS: " << std::fixed << std::setprecision(2) << 1000 / work_time.count());
			}

		m_renderer->wait_Idle();
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

	bool application::on_window_close(window_close_event& e) {

		m_running = false;
		return true;
	}

	bool application::on_window_resize(window_resize_event& event) {

		m_renderer->set_size(event.get_width(), event.get_height());
		return true;
	}

	bool application::on_window_refresh(window_refresh_event& e) {
		
		// Simple FPS controller - needs work
		m_frame_end = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> refresh_time = m_frame_end - m_frame_start;
		//CORE_LOG(Trace, "Refrech_time: " << std::fixed << std::setprecision(2) << refrech_time.count() << " refrech_time FPS: " << std::fixed << std::setprecision(2) << 1000 / refrech_time.count());

		m_renderer->refresh(static_cast<f32>(refresh_time.count()));
		m_frame_start = std::chrono::system_clock::now();
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
