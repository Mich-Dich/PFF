#pragma once

#include "util/util.h"

#include "engine/platform/window.h"

namespace PFF {

	class PFF_API application {
	public:
		application();
		virtual ~application();

		void run();
		void on_event();

		virtual bool initalize();
		virtual bool update(f64 delta_time);
		virtual bool render(f64 delta_time);

	private:

		std::unique_ptr<window> m_window;
		bool m_running = true;
	};

	static application* s_instance;

	// to be defined in Client
	application* create_application();
}
