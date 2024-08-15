#pragma once

#include "engine/layer/layer.h"
#include "engine/io_handler/input_mapping.h"
#include "editor_window.h"
#include "world_viewport.h"

struct ImGuiContext;

namespace PFF {
	
	class renderer;
	//class content_browser;

	class editor_layer : public layer {
	public:

		editor_layer(ImGuiContext* context);
		~editor_layer();

		template<typename window_type, typename... Args>
		void add_window(Args&&... args) {
			static_assert(std::is_base_of<editor_window, window_type>::value, "[window_type] must derive from editor_window");
			m_editor_windows.emplace_back(create_scoped_ref<window_type>(std::forward<Args>(args)...));
		}

		void on_attach() override;
		void on_detach() override;
		void on_update(f32 delta_time) override;
		void on_event(event& event) override;
		void on_imgui_render() override;

	private:

		void serialize(serializer::option option);
		
		std::vector<scope_ref<editor_window>> m_editor_windows{};		// generic

		// ImGui windows
		void window_main_title_bar();
		void window_main_viewport();
		void window_main_content();
		void window_editor_settings();
		void window_general_settings();
		void main_menu_bar();

		bool m_show_main_menu_bar = false;
		bool m_show_options = false;
		bool m_show_content_browser_0 = true;
		bool m_show_content_browser_1 = false;
		bool m_show_graphics_engine_settings = false;
		bool m_show_editor_settings = false;
		bool m_show_general_settings = false;

#ifdef PFF_EDITOR_DEBUG
		bool style_editor = false;
		bool demo_window = true;
#endif

		world_viewport_window				m_world_viewport_window{};

		std::vector<VkPresentModeKHR>		m_swapchain_supported_presentmodes;
		std::vector<const char*>			m_swapchain_supported_presentmodes_str;

		ImGuiContext*						m_context;
		f32									m_font_size = 16.0f;
		f32									m_titlebar_height = 60.f;
	};
}
