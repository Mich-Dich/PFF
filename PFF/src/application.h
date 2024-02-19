#pragma once

#include "engine/layer/layer_stack.h"			// need to include this for some reason
#include "engine/layer/imgui_layer.h"
#include "engine/layer/world_layer.h"
#include "util/timer.h"

namespace PFF {

	class pff_window;
	class renderer;
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

		FORCEINLINE f64 get_delta_time() const { return m_delta_time; }
		FORCEINLINE static application& get() { return *s_instance; }
		FORCEINLINE imgui_layer* get_imgui_layer() const { return m_imgui_layer; }
		FORCEINLINE world_layer* get_world_layer() const { return m_world_layer; }
		FORCEINLINE std::shared_ptr<pff_window> get_window() const { return m_window; }
		FORCEINLINE std::shared_ptr<renderer> get_renderer() const { return m_renderer; }
		FORCEINLINE std::shared_ptr<game_map> get_current_map() { return m_world_layer->get_current_map(); }
		FORCEINLINE void close_application() { m_running = false; }
		FORCEINLINE USE_IN_EDITOR void push_overlay(layer* overlay) { m_layerstack.push_overlay(overlay); }
		FORCEINLINE u32 get_target_fps() const { return m_target_fps; }
		FORCEINLINE bool get_limit_fps() const { return m_limit_fps; }
		FORCEINLINE bool is_titlebar_hovered() const { return m_is_titlebar_hovered; }
		FORCEINLINE void set_titlebar_hovered(bool value) { m_is_titlebar_hovered = value; LOG(Trace, "title bar hoverd: " << util::bool_to_str(value)); }

		FORCEINLINE void capture_cursor();
		FORCEINLINE void release_cursor();

		void run();
		virtual bool init();								// to be used by client
		virtual bool update(const f32 delta_time);			// potentally make private - every actor has own function (like UNREAL)
		virtual bool render(const f32 delta_time);			// potentally make private - every actor has own function (like UNREAL)
		virtual bool shutdown();							// to be used by client
		void register_player_controller(std::shared_ptr<player_controller> player_controller) { m_world_layer->register_player_controller(player_controller); }

		void limit_fps(const bool new_value, const u32 new_limit = 60);

	private:

		void on_event(event& event);
		bool on_window_close(window_close_event& event);
		bool on_window_resize(window_resize_event& event);
		bool on_window_refresh(window_refresh_event& event);
		bool on_window_focus(window_focus_event& event);

		//std::unique_ptr<basic_mesh> createCubeModel(glm::vec3 offset);

		static application* s_instance;
		imgui_layer* m_imgui_layer;
		world_layer* m_world_layer;
		layer_stack m_layerstack{};
		std::vector<event> m_event_queue;		// TODO: change to queue

		std::shared_ptr<game_map> m_current_map = nullptr;
		std::shared_ptr<pff_window> m_window = nullptr;
		std::shared_ptr<renderer> m_renderer;

		bool m_is_titlebar_hovered = false;
		bool m_focus = true;
		bool m_running = true;

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
