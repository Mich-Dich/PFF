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
		void window_main_menu_bar();
		void window_general_debugger();
		void window_main_content();
		void window_outliner();
		void window_details();
		void window_world_settings();
		void window_content_browser_0();
		void window_content_browser_1();

		void window_graphics_engine_settings();
		void window_editor_settings();
		void window_general_settings();

		void set_next_window_pos(int16 location);
		void progressbar_with_text(f32 percent, const char* text, f32 min_size_x = 0.0f, f32 min_size_y = 0.0f);
		void progressbar(f32 percent, f32 min_size_x = 0.0f, f32 min_size_y = 0.0f);
		void main_menu_bar();

		bool m_show_options = false;
		bool m_show_general_debugger = true;
		bool m_show_main_menu_bar = false;
		bool m_show_outliner = false;
		bool m_show_details = false;
		bool m_show_world_settings = false;
		bool m_show_content_browser_0 = false;
		bool m_show_content_browser_1 = false;
		bool m_show_graphics_engine_settings = false;
		bool m_show_editor_settings = false;
		bool m_show_general_settings = false;

		std::vector<VkPresentModeKHR> m_swapchain_supported_presentmodes;
		std::vector<const char*> m_swapchain_supported_presentmodes_str;

		ImGuiContext* m_context;
		f32 m_font_size = 14.5f;
	};

}
