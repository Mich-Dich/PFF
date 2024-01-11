#pragma once

#include "engine/layer/layer_stack.h"			// need to include this for some reason
#include "engine/layer/imgui_layer.h"

namespace PFF {

	class pff_window;
	class renderer;
	class event;
	class window_resize_event;
	class window_close_event;
	class window_refresh_event;
	class window_focus_event;
	class game_map;
	class timer;

	class PFF_API application {
	public:
		application();
		virtual ~application();

		DELETE_COPY(application);

		FORCEINLINE static application& get() { return *s_instance; }
		FORCEINLINE imgui_layer* get_imgui_layer() const { return m_imgui_layer; }
		FORCEINLINE std::shared_ptr<pff_window> get_window() const { return m_window; }
		FORCEINLINE std::shared_ptr<renderer> get_renderer() const { return m_renderer; }
		FORCEINLINE std::shared_ptr<game_map> get_current_map() { return m_current_map; }

		void run();
		virtual bool init();								// to be used by client
		virtual bool update(f32 delta_time);				// potentally make private - every actor has own function (like UNREAL)
		virtual bool render(f32 delta_time);				// potentally make private - every actor has own function (like UNREAL)
		virtual bool shutdown();							// to be used by client

	private:

		void on_event(event& event);
		bool on_window_close(window_close_event& event);
		bool on_window_resize(window_resize_event& event);
		bool on_window_refresh(window_refresh_event& event);
		bool on_window_focus(window_focus_event& event);

		static application* s_instance;
		imgui_layer* m_imgui_layer;
		layer_stack m_layerstack{};
		std::shared_ptr<game_map> m_current_map = nullptr;
		std::shared_ptr<pff_window> m_window = nullptr;
		std::shared_ptr<renderer> m_renderer;

		bool m_focus = true;
		bool m_running = true;

		u32 m_target_fps = 144;
		u32 m_fps = 0;
		u32 m_nonefocus_fps = 20;
		f32 m_delta_time = 0.0f;
		f64 m_work_time = 0, m_sleep_time = 0;
		std::unique_ptr<timer> fps_timer{};
	};


	// to be defined in Client
	application* create_application();
}
