
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
#include "engine/game_objects/camera.h"

#include "engine/render/renderer.h"
#include "engine/render/render_util.h"
#include "engine/render/vulkan/vk_renderer.h"

// ================= IN DEV =================
#include "project/script_system.h"
// ================= IN DEV =================

#include "application.h"

namespace PFF {

	project_data serialize_projects_data(const std::filesystem::path& project_path, serializer::option option) {

		project_data data{};
		for (const auto& entry : std::filesystem::directory_iterator(project_path)) {

			if (entry.is_directory() || entry.path().extension() != PFF_PROJECT_EXTENTION)
				continue;

			serializer::yaml(entry.path(), "project_data", option)
				.entry(KEY_VALUE(data.display_name))
				.entry(KEY_VALUE(data.name))
				//.entry(KEY_VALUE(data.ID))
				.entry(KEY_VALUE(data.engine_version))
				.entry(KEY_VALUE(data.project_version))
				.entry(KEY_VALUE(data.build_path))
				.entry(KEY_VALUE(data.start_world))
				.entry(KEY_VALUE(data.editor_start_world))
				.entry(KEY_VALUE(data.last_modified))
				.entry(KEY_VALUE(data.description))
				.vector("tags", data.tags, [&](serializer::yaml& inner, u64 x) {
					inner.entry("tag", data.tags[x]);
				});

			break;
		}

		return data;
	}



	// ==================================================================== setup ====================================================================

	application* application::s_instance = nullptr;

	//world_layer* application::m_world_layer;
	ref<pff_window> application::m_window;
	bool application::m_is_titlebar_hovered;
	bool application::m_running;


	application::application() {

		PFF_PROFILE_BEGIN_SESSION("startup", "benchmarks", "PFF_benchmark_startup.json");
		PFF_PROFILE_FUNCTION();

		PFF::logger::init("[$B$T:$J$E] [$B$L$X $I - $P:$G$E] $C$Z");
		CORE_ASSERT(!s_instance, "", "Application already exists");

		CORE_LOG_INIT();
		s_instance = this;

		// ---------------------------------------- general subsystems ----------------------------------------

		// ======================================= DEV force load project dir ======================================= 
		serializer::yaml(config::get_filepath_from_configtype(util::get_executable_path(), config::file::editor), "editor_data", serializer::option::load_from_file)
			.entry("last_opened_project", m_project_path);


		// ========================================== DEV-ONLY should be set by launcher or when building the project_solution ==========================================
		if (m_project_path.empty()) {

			const std::vector<std::pair<std::string, std::string>> project_file_filters = { {"PFF Project File", "*.pffproj"} };
			m_project_path = util::file_dialog("Open PFF-Project", project_file_filters).parent_path();
		}

		serializer::yaml(config::get_filepath_from_configtype(util::get_executable_path(), config::file::editor), "editor_data", serializer::option::save_to_file)
			.entry("last_opened_project", m_project_path);
		// ========================================== DEV-ONLY should be set by launcher or when building the project_solution ==========================================


		if (!m_project_path.empty())
			m_project_data = serialize_projects_data(m_project_path, serializer::option::load_from_file);

		// ---------------------------------------- general subsystems ----------------------------------------
		config::init(m_project_path, util::get_executable_path());

		serialize(serializer::option::load_from_file);							// LOAD PROJECT_DATA
		set_fps_settings(m_target_fps);

		m_layerstack = create_ref<layer_stack>();

		m_window = std::make_shared<pff_window>();
		m_window->set_event_callback(BIND_FUNKTION(application::on_event));

		// ---------------------------------------- renderer ----------------------------------------

		GET_RENDERER.setup(m_window, m_layerstack);
		
		// ---------------------------------------- layers ----------------------------------------
		m_world_layer = new world_layer();
		m_layerstack->push_layer(m_world_layer);
		GET_RENDERER.set_active_camera(m_world_layer->get_editor_camera());

		m_imgui_layer = new UI::imgui_layer();
		m_layerstack->push_overlay(m_imgui_layer);

		script_system::init();

		// TODO: load the map specefied in the project settings as editor_start_world
		m_world_layer->set_map(create_ref<map>());


		PFF_PROFILE_END_SESSION();
	}

	application::~application() {

		PFF_PROFILE_BEGIN_SESSION("shutdown", "benchmarks", "PFF_benchmark_shutdown.json");
		
		GET_RENDERER.set_state(system_state::inactive);
		serialize(serializer::option::save_to_file);

		for (size_t x = 0; x < m_timers.size(); x++)
			util::cancel_timer(m_timers[x]);
		m_timers.clear();

		GET_RENDERER.resource_free();

		m_layerstack->pop_overlay(m_imgui_layer);
		delete m_imgui_layer;
		m_imgui_layer = nullptr;

		m_layerstack->pop_overlay(m_world_layer);
		delete m_world_layer;
		m_world_layer = nullptr;

		GET_RENDERER.shutdown();
		
		m_layerstack.reset();
		m_window.reset();
		CORE_LOG_SHUTDOWN();
		PFF_PROFILE_END_SESSION();
	}

	// ==================================================================== main loop ====================================================================

	std::future<void>& application::add_future(std::future<void>& future, std::shared_ptr<std::pair<std::atomic<bool>, std::condition_variable>>& shared_state) {

		std::lock_guard<std::mutex> lock(m_global_futures_mutex);
		m_timers.push_back({ std::move(future), shared_state });
		return m_timers.back().future;
	}


	void application::remove_timer(std::future<void>& future) {

		std::lock_guard<std::mutex> lock(m_global_futures_mutex);

		LOG(Trace, "timer vector length: " << m_timers.size());
		/*m_timers.erase(std::remove_if(m_timers.begin(), m_timers.end(),
			[&future](const util::timer& t) { return t.future._Ptr() == future._Ptr(); }),
			m_timers.end());*/
		LOG(Trace, "timer vector length: " << m_timers.size());
	}



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
			GET_RENDERER.draw_frame(m_delta_time);	// engine render

			limit_fps();
		}

		GET_RENDERER.wait_idle();
		
		CORE_ASSERT(shutdown(), "client application is shutdown", "client-defint shutdown() has failed");			// init user code / potentally make every actor have own function (like UNREAL)

		PFF_PROFILE_END_SESSION();
	}

	// ==================================================================== PUBLIC ====================================================================

	void application::set_fps_settings(const bool set_for_engine_focused, const u32 new_limit) {

		if (set_for_engine_focused)
			m_target_fps = new_limit;
		else
			m_nonefocus_fps = new_limit;

		set_fps_settings(m_focus ? m_target_fps : m_nonefocus_fps);
	}

	void application::limit_fps(const bool new_value, const u32 new_limit) {

		m_limit_fps = new_value;
		set_fps_settings(math::max(new_limit, (u32)1 ));
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
		GET_RENDERER.set_state(system_state::active);
		m_is_titlebar_hovered = false;
		m_running = true;
		m_window->show_window(true);
		m_window->poll_events();
		start_fps_measurement();
	}

	void application::client_shutdown() {

	}

	void application::serialize(serializer::option option) {

		serializer::yaml(config::get_filepath_from_configtype(application::get().get_project_path(), config::file::engine), "FPS_control", option)
			.entry(KEY_VALUE(m_limit_fps))
			.entry(KEY_VALUE(m_target_fps))
			.entry(KEY_VALUE(m_nonefocus_fps));
	}

	void application::set_fps_settings(u32 target_fps) { target_duration = static_cast<f32>(1.0 / target_fps); }

	void application::start_fps_measurement() { m_last_frame_time = static_cast<f32>(glfwGetTime()); }

	void application::end_fps_measurement(f32& work_time) { work_time = static_cast<f32>(glfwGetTime()) - m_last_frame_time; }

	void application::limit_fps() {

		//m_limit_fps, m_fps, m_delta_time, m_work_time, m_sleep_time

		m_work_time = static_cast<f32>(glfwGetTime()) - m_last_frame_time;
		if (m_work_time < target_duration && m_limit_fps) {

			PFF_PROFILE_SCOPE("sleep");
			m_sleep_time = (target_duration - m_work_time) * 1000;
			PFF::util::high_precision_sleep(m_sleep_time / 1000);
		} else
			m_sleep_time = 0;

		f32 time = static_cast<f32>(glfwGetTime());
		m_delta_time = std::min<f32>(time - m_last_frame_time, 100000);
		m_last_frame_time = time;
		m_fps = static_cast<u32>(1.0 / (m_work_time + (m_sleep_time * 0.001)) + 0.5); // Round to nearest integer
	}

	// ==================================================================== event handling ====================================================================

	void application::on_event(event& event) {

		PFF_PROFILE_FUNCTION();

		// application events
		event_dispatcher dispatcher(event);
		dispatcher.dispatch<window_close_event>(BIND_FUNKTION(application::on_window_close));
		dispatcher.dispatch<window_resize_event>(BIND_FUNKTION(application::on_window_resize));
		dispatcher.dispatch<window_refresh_event>(BIND_FUNKTION(application::on_window_refresh));
		dispatcher.dispatch<window_focus_event>(BIND_FUNKTION(application::on_window_focus));

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

		GET_RENDERER.set_size(event.get_width(), event.get_height());

		// m_camera.set_orthographic_projection(-aspect, aspect, -1, 1, 0, 10);
		// m_camera.set_view_YXZ(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(0.0f));
		// m_camera.set_view_direction(glm::vec3{ 0.0f }, glm::vec3{ 0.5f, 0.0f, 1.0f });
		//m_world_layer->get_editor_camera()->set_aspect_ratio(vk_renderer->get_aspect_ratio());

		return true;
	}

	bool application::on_window_refresh(window_refresh_event& event) {

		PFF_PROFILE_FUNCTION();

		limit_fps();
		// m_imgui_layer->set_fps_values(m_limit_fps, (m_focus ? m_target_fps : m_nonefocus_fps), m_fps, static_cast<f32>(m_work_time * 1000), static_cast<f32>(m_sleep_time * 1000));
		GET_RENDERER.refresh(m_delta_time);
		return true;
	}

	bool PFF::application::on_window_focus(window_focus_event& event) {

		PFF_PROFILE_FUNCTION();

		m_focus = event.get_focus();
		if (event.get_focus())
			set_fps_settings(m_target_fps);
		else
			set_fps_settings(m_nonefocus_fps);

		return true;
	}

}
