#pragma once

#include "engine/layer/layer_stack.h"			// need to include this for some reason

namespace PFF {

	class pff_window;
	class vulkan_renderer;
	class event;
	class window_resize_event;
	class window_close_event;
	class window_refresh_event;
	class game_map;

	class PFF_API application {
	public:
		application();
		virtual ~application();

		void run();
		virtual bool initalize();							// to be used by client
		virtual bool update(f32 delta_time);				// to be used by client
		virtual bool render(f32 delta_time);				// to be used by client

		DELETE_COPY(application);

	private:

		void on_event(event& event);
		bool on_window_resize(window_resize_event& event);
		bool on_window_close(window_close_event& e);
		bool on_window_refresh(window_refresh_event& e);
		void push_layer(layer* layer);						// client doesnt need to be consernt with layer-system, just tick a box if you want debug stuff
		void push_overlay(layer* overlay);					// client doesnt need to be consernt with layer-system, just tick a box if you want debug stuff

		layer_stack m_layerstack{};
		std::shared_ptr<pff_window> m_window{};
		std::shared_ptr<vulkan_renderer> m_vulkan_renderer{};
		f32 m_delta_time = 1;
		bool m_running = true;
	};

	static application* s_instance;

	// to be defined in Client
	application* create_application();
}
