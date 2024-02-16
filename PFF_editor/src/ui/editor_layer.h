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

		// ImGui windows
		void ImGui_window_main_menu_bar();
		void ImGui_window_general_debugger();
		void ImGui_window_Outliner();
		void ImGui_window_Details();
		void ImGui_window_World_Settings();
		void ImGui_window_ContentBrowser_0();
		void ImGui_window_ContentBrowser_1();

		void set_next_window_pos(int16 location);
		void progressbar_with_text(f32 percent, const char* text, f32 min_size_x = 0.0f, f32 min_size_y = 0.0f);
		void progressbar(f32 percent, f32 min_size_x = 0.0f, f32 min_size_y = 0.0f);

		bool m_show_options = false;
		bool m_show_general_debugger = true;
		bool m_show_main_menu_bar = false;
		bool m_show_Outliner = false;
		bool m_show_Details = false;
		bool m_show_World_Settings = false;
		bool m_show_ContentBrowser_0 = false;
		bool m_show_ContentBrowser_1 = false;

		ImGuiContext* m_context;
		f32 m_font_size = 14.5f;
	};

}
