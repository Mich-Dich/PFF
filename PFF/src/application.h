#pragma once

#include "util/pffpch.h"

#include "engine/layer/layer_stack.h"			// need to include this for some reason
#include "engine/layer/imgui_layer.h"
#include "engine/layer/world_layer.h"
#include "engine/render/renderer.h"
#include "util/timer.h"

namespace PFF {

	class pff_window;
	// class renderer;
	class event;
	class window_resize_event;
	class window_close_event;
	class window_refresh_event;
	class window_focus_event;
	class game_map;
	class camera;

	class PFF_API application {
	public:
		application();
		virtual ~application();

		DELETE_COPY(application);

		FORCEINLINE f64 get_delta_time() const							{ return m_delta_time; }
		FORCEINLINE USE_IN_EDITOR void push_overlay(layer* overlay)		{ m_layerstack.push_overlay(overlay); }
		FORCEINLINE USE_IN_EDITOR void pop_overlay(layer* overlay)		{ m_layerstack.pop_overlay(overlay); }
		FORCEINLINE u32 get_target_fps() const							{ return m_target_fps; }
		FORCEINLINE bool get_limit_fps() const							{ return m_limit_fps; }
		FORCEINLINE void set_titlebar_hovered(bool value)				{ m_is_titlebar_hovered = value; }

		// static
		FORCEINLINE static application& get()							{ return *s_instance; }
		FORCEINLINE static imgui_layer* get_imgui_layer()				{ return m_imgui_layer; }
		FORCEINLINE static world_layer* get_world_layer()				{ return m_world_layer; }
		FORCEINLINE static ref<game_map> get_current_map()				{ return m_world_layer->get_current_map(); }
		FORCEINLINE static ref<pff_window> get_window()					{ return m_window; }
		FORCEINLINE static ref<renderer> get_renderer()					{ return m_renderer; }
		FORCEINLINE static void  set_render_state(system_state state)	{ return m_renderer->set_state(state); }
		FORCEINLINE static void close_application()						{ m_running = false; }
		FORCEINLINE static bool is_titlebar_hovered()					{ return m_is_titlebar_hovered; }

		void run();
		void limit_fps(const bool new_value, const u32 new_limit = 60);
		FORCEINLINE void capture_cursor();
		FORCEINLINE void release_cursor();
		FORCEINLINE void minimize_window();
		FORCEINLINE void restore_window();
		FORCEINLINE void maximize_window();

		void register_player_controller(ref<player_controller> player_controller) { m_world_layer->register_player_controller(player_controller); }
		void get_fps_values(bool& limit_fps, u32& target_fps, u32& current_fps, f32& work_time, f32& sleep_time);

		virtual bool init();								// to be used by client
		virtual bool update(const f32 delta_time);			// potentally make private - every actor has own function (like UNREAL)
		virtual bool render(const f32 delta_time);			// potentally make private - every actor has own function (like UNREAL)
		virtual bool shutdown();							// to be used by client

	private:

		void on_event(event& event);
		bool on_window_close(window_close_event& event);
		bool on_window_resize(window_resize_event& event);
		bool on_window_refresh(window_refresh_event& event);
		bool on_window_focus(window_focus_event& event);

		//scope_ref<basic_mesh> createCubeModel(glm::vec3 offset);

		static application* s_instance;
		static ref<renderer> m_renderer;
		static ref<pff_window> m_window;
		static bool m_is_titlebar_hovered;
		static bool m_running;
		static imgui_layer* m_imgui_layer;
		static world_layer* m_world_layer;

		layer_stack m_layerstack{};
		std::vector<event> m_event_queue;		// TODO: change to queue
		ref<game_map> m_current_map = nullptr;

		bool m_focus = true;
		f32 m_last_frame_time = 0.f;
		bool m_limit_fps = true;
		u32 m_target_fps = 60;
		u32 m_nonefocus_fps = 30;
		u32 m_fps{};
		f32 m_delta_time = 0.f;
		f32 m_work_time{}, m_sleep_time{};
		timer fps_timer;
	};


	// to be defined in Client
	application* create_application();
}
