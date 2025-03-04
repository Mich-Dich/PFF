#pragma once

#include <imgui.h>

#include "util/system.h"
#include "util/io/serializer_data.h"
#include "util/io/config.h"

#include "engine/layer/layer.h"

namespace PFF::UI {

	#define LERP_GRAY(value)					{value, value, value, 1.f }
	#define IMCOLOR_GRAY(value)					ImColor{value, value, value, 255 }

	enum class window_pos;

	FORCEINLINE static u32 convert_color_to_int(const ImVec4& color) { return IM_COL32(255 * color.x, 255 * color.y, 255 * color.z, 255 * color.w); }

	enum class theme_selection : u8 {
		dark,
		light
	};

	static std::filesystem::path ini_file_location = PFF::config::get_filepath_from_configtype_ini(util::get_executable_path(), config::file::imgui).c_str();

	static f32  m_font_size = 15.f, m_font_size_header_0 = 19.f, m_font_size_header_1 = 23.f, m_font_size_header_2 = 27.f, m_big_font_size = 18.f;
	static theme_selection UI_theme = theme_selection::dark;
	static bool window_border{};
	static ImVec4 highlited_window_bg = LERP_GRAY(0.57f);
	static f32 default_item_width{};

	PFF_GETTER_REF_FUNC(ImVec4, main_color);
	PFF_GETTER_REF_FUNC(ImVec4, main_titlebar_color);

	PFF_GETTER_REF_FUNC(ImVec4, action_color_00_faded);
	PFF_GETTER_REF_FUNC(ImVec4, action_color_00_weak);
	PFF_GETTER_REF_FUNC(ImVec4, action_color_00_default);
	PFF_GETTER_REF_FUNC(ImVec4, action_color_00_hover);
	PFF_GETTER_REF_FUNC(ImVec4, action_color_00_active);

	PFF_GETTER_REF_FUNC(ImVec4, default_gray);
	PFF_GETTER_REF_FUNC(ImVec4, default_gray_1);

	PFF_GETTER_REF_FUNC(ImVec4, action_color_gray_default);
	PFF_GETTER_REF_FUNC(ImVec4, action_color_gray_hover);
	PFF_GETTER_REF_FUNC(ImVec4, action_color_gray_active);

	void set_UI_theme_selection(theme_selection theme_selection);
	void enable_window_border(bool enable);
	void update_UI_theme();
	void update_UI_colors(ImVec4 new_color);


	class imgui_layer : public layer {
	public:

		imgui_layer();
		~imgui_layer();

		PFF_DEFAULT_GETTER(ImGuiContext*, context)
		PFF_DEFAULT_GETTER_SETTER_ALL(bool, show_FPS_window);
		PFF_DEFAULT_GETTER_SETTER_ALL(bool, show_renderer_metrik);
		//FORCEINLINE bool is_render_clear_enabled() const { return m_clear_enabled; }

		void on_attach() override;
		void on_detach() override;
		void on_update(const f32 delta_time) override;
		void on_event(event& event) override;
		void on_imgui_render() override;
		void serialize(PFF::serializer::option option);
		
		void show_renderer_metrik();
		void show_FPS();
		
		ImFont* get_font(const std::string& name = "default");

		ImGuiID m_viewport_ID{};

	private:


		// ------------------------- general -------------------------
		ImGuiContext* m_context{};
		std::unordered_map<std::string, ImFont*> m_fonts{};

		// ------------------------- performance display -------------------------
		bool				m_show_FPS_window = true;
		bool				m_show_renderer_metrik = true;
		f32					m_work_time = 0.f, m_sleep_time = 0.f;
		u32					m_target_fps = 0, m_current_fps = 0;
		bool				m_limit_fps = false;
		bool				fonts_need_recreation = false;

		UI::window_pos		renderer_metrik_window_location = static_cast< window_pos>(2);
		UI::window_pos		FPS_window_location = static_cast< window_pos>(2);
	};
}
