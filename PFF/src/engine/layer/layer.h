#pragma once

#include "engine/events/event.h"

namespace PFF {

	class layer {
	public:
		layer(const std::string& name = "layer");
		virtual ~layer();

		virtual void on_attach();
		virtual void on_detach();
		virtual void on_update();
		virtual void on_event(event& event);
		virtual void on_imgui_render();

		FORCEINLINE const  std::string& get_name() const { return m_debugname; }

	private:

		std::string m_debugname;
		bool m_enabled;
	};

}