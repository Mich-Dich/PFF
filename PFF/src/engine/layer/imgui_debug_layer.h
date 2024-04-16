#pragma once

#include "engine/layer/layer.h"

#include "imgui.h"

namespace PFF {
	
	class imgui_debug_layer : public layer {
	public:

		imgui_debug_layer();
		~imgui_debug_layer();

		//FORCEINLINE bool is_render_clear_enabled() const { return m_clear_enabled; }

		//PFF_API_EDITOR ImFont* get_font(const std::string& name);

		void on_attach() override;
		void on_detach() override;
		void on_update(const f32 delta_time) override;
		void on_event(event& event) override;
		void on_imgui_render() override;
		void show_performance();

	private:
		
		void serialize(serializer::option option);

		// performance display
		bool m_show_performance_window = true;
		f32 m_work_time = 0.f, m_sleep_time = 0.f;
		u32 m_target_fps = 0, m_current_fps = 0;
		bool m_limit_fps = false;
						
	};
}
