#pragma once

#include <imgui.h>

#include "engine/layer/layer.h"
//#include "util/io/serializer.h"

namespace PFF::UI {

	FORCEINLINE static u32 convert_color_to_int(const ImVec4& color) { return IM_COL32(255 * color.x, 255 * color.y, 255 * color.z, 255 * color.w); }

	enum class theme_selection {
		dark,
		light
	};


	static std::string ini_file_location = config::get_filepath_from_configtype_ini(config::file::imgui).c_str();

	static f32 m_font_size = 14.5f, m_big_font_size = 18.f;
	static theme_selection UI_theme = theme_selection::dark;
	static bool enable_window_forder;
	static ImVec4 highlited_window_bg;
	static f32 default_item_width;

	static ImVec4 main_color;
	static ImVec4 main_titlebar_color;

	static ImVec4 action_color_00_faded;
	static ImVec4 action_color_00_weak;
	static ImVec4 action_color_00_default;
	static ImVec4 action_color_00_hover;
	static ImVec4 action_color_00_active;
	
	static ImVec4 action_color_gray_default;
	static ImVec4 action_color_gray_hover;
	static ImVec4 action_color_gray_active;

	void PFF_API set_UI_theme_selection(theme_selection theme_selection);
	void PFF_API enable_window_border(bool enable);
	void PFF_API update_UI_theme();
	void PFF_API update_UI_colors(ImVec4 new_color);


	class imgui_layer : public layer {
	public:

		imgui_layer();
		~imgui_layer();

		FORCEINLINE ImGuiContext* get_context() { return m_context; }
		//FORCEINLINE bool is_render_clear_enabled() const { return m_clear_enabled; }

		void on_attach() override;
		void on_detach() override;
		void on_update(const f32 delta_time) override;
		void on_event(event& event) override;
		void on_imgui_render() override;
		void PFF_API show_FPS();
		
		PFF_API_EDITOR ImFont* get_font(const std::string& name = "default");

		ImGuiID m_viewport_ID{};

	private:

		// ------------------------- general -------------------------
		ImGuiContext* m_context{};
		std::unordered_map<std::string, ImFont*> m_fonts{};

		// ------------------------- performance display -------------------------
		bool m_show_FPS_window = true;
		f32 m_work_time = 0.f, m_sleep_time = 0.f;
		u32 m_target_fps = 0, m_current_fps = 0;
		bool m_limit_fps = false;
		bool fonts_need_recreation = false;

	};
}
