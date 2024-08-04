#pragma once

#include "util/pffpch.h"

#include "engine/layer/layer_stack.h"			// need to include this for some reason
#include "engine/layer/imgui_layer.h"
#include "engine/layer/world_layer.h"
//#include "engine/render/renderer.h"

#include "engine/render/renderer.h"
#include "engine/render/vulkan/vk_renderer.h"

namespace PFF {

	class pff_window;
	class renderer;
	class event;
	class window_resize_event;
	class window_close_event;
	class window_refresh_event;
	class window_focus_event;
	class map;
	class camera;

	class PFF_API application {
	public:

		application();
		virtual ~application();

		PFF_DELETE_COPY_CONSTRUCTOR(application);

		PFF_DEFAULT_GETTER_C(f64,											delta_time)
		PFF_DEFAULT_GETTER_C(u32,											target_fps)
		PFF_DEFAULT_GETTERS(u32,											target_fps)
		PFF_DEFAULT_GETTERS(u32,											nonefocus_fps)
		PFF_DEFAULT_GETTERS(bool,											limit_fps)
		PFF_DEFAULT_GETTER(static ref<pff_window>,							window);
		PFF_DEFAULT_GETTER(UI::imgui_layer*,								imgui_layer);
		PFF_DEFAULT_GETTER(world_layer*,									world_layer);

		PFF_DEFAULT_GETTER_SETTER(bool,										is_titlebar_hovered)
			
		FORCEINLINE static application& get()								{ return *s_instance; }
		FORCEINLINE static void close_application()							{ m_running = false; }
		FORCEINLINE static RENDERER& get_renderer()							{ return GET_RENDERER; }
		FORCEINLINE static void set_render_state(system_state state)		{ GET_RENDERER.set_state(state); }

		FORCEINLINE USE_IN_EDITOR void push_overlay(layer* overlay)			{ m_layerstack->push_overlay(overlay); }
		FORCEINLINE USE_IN_EDITOR void pop_overlay(layer* overlay)			{ m_layerstack->pop_overlay(overlay); }

		std::future<void>& add_future(std::future<void>& future, std::shared_ptr<std::pair<std::atomic<bool>, std::condition_variable>>& shared_state);
		void remove_timer(std::future<void>& future);
		void run();

		// [set_target_fps] if true: set the fps-limit for if the window is focused, if false: set the fps-limit for if the window is not focused
		void set_fps_settings(const bool set_for_engine_focused, const u32 new_limit);

		void limit_fps(const bool new_value, const u32 new_limit);
		FORCEINLINE void capture_cursor();
		FORCEINLINE void release_cursor();
		FORCEINLINE void minimize_window();
		FORCEINLINE void restore_window();
		FORCEINLINE void maximize_window();

		void register_player_controller(ref<player_controller> player_controller) { m_world_layer->register_player_controller(player_controller); }
		void get_fps_values(bool& limit_fps, u32& target_fps, u32& current_fps, f32& work_time, f32& sleep_time);

		// ---------------------- client defined ---------------------- 
		virtual bool init()							{ return true; };
		virtual bool update(const f32 delta_time)	{ return true; };
		virtual bool render(const f32 delta_time)	{ return true; };
		virtual bool shutdown()						{ return true; };

		// ---------------------- fps control ---------------------- 
		void set_fps_settings(u32 target_fps);
		void start_fps_measurement();
		void end_fps_measurement(f32& work_time);
		void limit_fps();

	private:

		void client_init();
		void client_shutdown();

		void serialize(serializer::option option);
		
		void on_event(event& event);
		bool on_window_close(window_close_event& event);
		bool on_window_resize(window_resize_event& event);
		bool on_window_refresh(window_refresh_event& event);
		bool on_window_focus(window_focus_event& event);

		static application* s_instance;

		static ref<pff_window> m_window;
		static bool m_is_titlebar_hovered;
		static bool m_running;
		UI::imgui_layer* m_imgui_layer;
		world_layer* m_world_layer;

		ref<layer_stack> m_layerstack{};
		std::vector<event> m_event_queue;		// TODO: change to queue
		ref<map> m_current_map = nullptr;

		bool m_focus = true;
		bool m_limit_fps = true;
		u32 m_target_fps = 60;
		u32 m_nonefocus_fps = 30;
		u32 m_fps{};
		f32 m_delta_time = 0.f;
		f32 m_work_time{}, m_sleep_time{};
		f32 target_duration{};
		f32 m_last_frame_time = 0.f;

		// vector to store futures (from timer_async and other std::async tasks)
		std::vector<util::timer> m_timers{};
		std::mutex m_global_futures_mutex{}; // Mutex to protect global_futures

	};

	// to be defined in Client
	application* create_application();
}
