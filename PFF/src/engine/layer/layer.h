#pragma once

#include "engine/events/event.h"

namespace PFF {

	class PFF_API_EDITOR layer {
	public:
		layer(const std::string& name = "layer");
		virtual ~layer();

		virtual void on_attach();
		virtual void on_detach();
		virtual void on_update(f32 delta_time);
		virtual void on_event(event& event);
		virtual void on_imgui_render();

		FORCEINLINE const  std::string& get_name() const { return m_debugname; }

	private:

		std::string m_debugname;
		bool m_enabled;
	};

}