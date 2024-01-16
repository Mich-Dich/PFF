#pragma once

#include "engine/layer/layer.h"
#include "engine/io_handler/input_mapping.h"

struct ImGuiContext;

namespace PFF {

	class renderer;

	class editor_layer : public layer {
	public:
		editor_layer(ImGuiContext* context) : m_context(context) {}
		~editor_layer();

		void on_attach() override;
		void on_detach() override;
		void on_update(f32 delta_time) override;
		void on_event(event& event) override;
		void on_imgui_render() override;

	private:

		void main_menu_bar();
		void general_debugger();
		void show_current_input();

		void display_value_bool(bool value);
		void display_value_num(f32 value);
		void display_value_vec2(glm::vec2 value);

		void set_next_window_pos(int16 location);
		void progressbar_with_text(f32 percent, const char* text, f32 min_size_x = 0.0f, f32 min_size_y = 0.0f);
		void progressbar(f32 percent, f32 min_size_x = 0.0f, f32 min_size_y = 0.0f);

		bool m_show_options = false;
		bool m_show_general_debugger = true;
		ImGuiContext* m_context;
		f32 m_font_size = 14.5f;
	};

}
