#pragma once

#include "util/util.h"

namespace PFF {

	class pff_window;
	class vulkan_renderer;

	class PFF_API application {
	public:
		application();
		virtual ~application();

		void run();
		/*
		void on_event();
		virtual bool initalize();
		virtual bool update(f64 delta_time);
		virtual bool render(f64 delta_time);*/

	private:

		std::shared_ptr<pff_window> m_window;
		std::shared_ptr<vulkan_renderer> m_vulkan_renderer;
		bool m_running = true;
	};

	static application* s_instance;

	// to be defined in Client
	application* create_application();
}
