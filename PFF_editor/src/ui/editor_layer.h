#pragma once

#include "engine/layer/layer.h"

namespace PFF {

	class renderer;

	class editor_layer : public layer {
	public:
		editor_layer();
		~editor_layer();

		void on_attach() override;
		void on_detach() override;
		void on_update(f32 delta_time) override;
		void on_event(event& event) override;
		void on_imgui_render() override;

	private:

		void set_next_window_pos(int16 location);
		void progressbar_with_text(f32 percent, const char* text, f32 min_size_x = 0.0f, f32 min_size_y = 0.0f);
		void progressbar(f32 percent, f32 min_size_x = 0.0f, f32 min_size_y = 0.0f);

		f32 m_font_size = 14.5f;
	};

}
