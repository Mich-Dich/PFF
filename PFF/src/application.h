#pragma once

#include "engine/game_objects/game_object.h"
#include "engine/map/game_map.h"

namespace PFF {

	class pff_window;
	class vulkan_renderer;
	class basic_mesh;
	class event;

	class PFF_API application {
	public:
		application();
		virtual ~application();

		void run();
		void on_event(event& event);
		void on_window_resize(event& event);

		virtual bool initalize();
		virtual bool update(f32 delta_time);
		virtual bool render(f32 delta_time);

		DELETE_COPY(application);

	private:

		std::shared_ptr<pff_window> m_window{};
		std::shared_ptr<game_map> m_map{};
		std::shared_ptr<vulkan_renderer> m_vulkan_renderer{};
		f32 m_delta_time = 1;
		bool m_running = true;
	};

	static application* s_instance;

	// to be defined in Client
	application* create_application();
}
