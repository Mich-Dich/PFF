#pragma once

#include "engine/events/event.h"

namespace PFF {

	class layer {
	public:

		layer(const std::string& name = "layer");
		virtual ~layer();

		FORCEINLINE const  std::string& get_name() const { return m_debugname; }

		virtual void on_attach();
		virtual void on_detach();
		virtual void on_update(const f32 delta_time = 0.f);
		virtual void on_event(event& event);
		virtual void on_imgui_render();

	private:

		std::string m_debugname;
		bool m_enabled;
	};

}
