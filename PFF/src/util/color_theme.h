#pragma once

#include "util/pffpch.h"
#include <imgui.h>

#include "util/imgui/imgui_util.h"

#define PFF_UI_ACTIVE_THEME				PFF::UI::THEME::current_theme

#define LERP_MAIN_COLOR_DARK(value)			{PFF::UI::THEME::main_color.x * value, PFF::UI::THEME::main_color.y * value, PFF::UI::THEME::main_color.z * value, 1.f }		// Set [w] to be [1.f] to disable accidental transparency
#define LERP_MAIN_COLOR_LIGHT(value)		{	(1.f - value) * 1.f + value * PFF::UI::THEME::main_color.x, \
												(1.f - value) * 1.f + value * PFF::UI::THEME::main_color.y, \
												(1.f - value) * 1.f + value * PFF::UI::THEME::main_color.z, \
												1.f }																		// Set [w] to be [1.f] to disable accidental transparency

#define LERP_GRAY(value)				{value, value, value, 1.f }
#define LERP_GRAY_A(value, alpha)		{value, value, value, alpha }
#define COLOR_INT_CONVERTION(color)		IM_COL32(255 * color.x, 255 * color.y, 255 * color.z, 255 * color.w)

namespace PFF {

	FORCEINLINE static u32 convert_color_to_int(const ImVec4& color) { return IM_COL32(255 * color.x, 255 * color.y, 255 * color.z, 255 * color.w); }

	template<typename key_type, typename value_type>
	bool contains(const std::unordered_map<key_type, value_type>& map, const key_type& key) {

		return map.find(key) != map.end();
	}

	namespace UI::THEME {

		enum class theme_selection {
			dark,
			light
		};

		static theme_selection UI_theme = theme_selection::dark;
		static ImVec4 main_color = { .03f,	.72f,	.0f,	1.f };
		static bool enable_window_forder = false;
		static ImVec4 highlited_window_bg = LERP_GRAY(0.17f);
		static ImVec4 main_titlebar_color = LERP_MAIN_COLOR_DARK(.5f);
		static f32 default_item_width = 200.f;
		
		static ImVec4 action_color_00_default	= LERP_MAIN_COLOR_DARK(0.7f);
		static ImVec4 action_color_00_hover		= LERP_MAIN_COLOR_DARK(0.85f);
		static ImVec4 action_color_00_active	= LERP_MAIN_COLOR_DARK(1.f);

		static ImVec4 action_color_gray_default	= LERP_GRAY(0.2f);
		static ImVec4 action_color_gray_hover	= LERP_GRAY(0.27f);
		static ImVec4 action_color_gray_active	= LERP_GRAY(0.35f);

#ifndef UI_THEME_IMPLEMENTATION
		
		void PFF_API serialize(serializer::option option);

		void PFF_API serialize_data(serializer::option option);
		void PFF_API enable_window_border(bool enable);
		void PFF_API update_UI_theme();
		void PFF_API update_UI_colors(ImVec4 new_color);

#else

		void PFF_API serialize(serializer::option option) {

			serializer::yaml(config::get_filepath_from_configtype(config::file::ui), "font", option)
				.entry(KEY_VALUE(PFF::UI::imgui::util::s_font_size))
				.entry(KEY_VALUE(PFF::UI::imgui::util::s_big_font_size));
		}

		void PFF_API serialize_data(serializer::option option) {

			serializer::yaml(config::get_filepath_from_configtype(config::file::ui), "theme", option)
				.entry(KEY_VALUE(UI_theme))
				.entry(KEY_VALUE(main_color))
				.entry(KEY_VALUE(enable_window_forder))
				.entry(KEY_VALUE(highlited_window_bg))
				.entry(KEY_VALUE(main_titlebar_color))
				.entry(KEY_VALUE(default_item_width))

				// color heightlight
				.entry(KEY_VALUE(action_color_00_default))
				.entry(KEY_VALUE(action_color_00_hover))
				.entry(KEY_VALUE(action_color_00_active))

				// gray
				.entry(KEY_VALUE(action_color_gray_default))
				.entry(KEY_VALUE(action_color_gray_hover))
				.entry(KEY_VALUE(action_color_gray_active));
		}

		void PFF_API enable_window_border(bool enable) {

			enable_window_forder = enable;
			serialize_data(serializer::option::save_to_file);

			ImGuiStyle* style = &ImGui::GetStyle();
			style->WindowBorderSize = enable ? 1.f : 0.f;
		}

		void PFF_API update_UI_theme() {

			ImGuiStyle* style = &ImGui::GetStyle();
			ImVec4* colors = style->Colors;

			// main sizes
			style->WindowPadding = ImVec2(10.f, 4.f);
			style->FramePadding = ImVec2(4.f, 4.f);
			style->CellPadding = ImVec2(4.f, 4.f);
			style->ItemSpacing = ImVec2(4.f, 4.f);
			style->ItemInnerSpacing = ImVec2(4.f, 4.f);
			style->TouchExtraPadding = ImVec2(4.f, 4.f);
			style->IndentSpacing = 10.f;
			style->ScrollbarSize = 14.f;
			style->GrabMinSize = 14.f;

			// border
			style->WindowBorderSize = 1.0f;
			style->ChildBorderSize = 0.0f;
			style->PopupBorderSize = 0.0f;
			style->FrameBorderSize = 0.0f;
			style->TabBorderSize = 0.0f;

			// padding
			style->WindowRounding = 2.f;
			style->ChildRounding = 2.f;
			style->FrameRounding = 2.f;
			style->PopupRounding = 2.f;
			style->ScrollbarRounding = 2.f;
			style->GrabRounding = 2.f;
			style->TabRounding = 2.f;

			switch (UI_theme) {
			case PFF::UI::THEME::theme_selection::dark:
			{

				action_color_00_default = LERP_MAIN_COLOR_DARK(.7f);
				action_color_00_hover = LERP_MAIN_COLOR_DARK(.85f);
				action_color_00_active = LERP_MAIN_COLOR_DARK(1.f);

				action_color_gray_default = LERP_GRAY(0.25f);
				action_color_gray_hover = LERP_GRAY(0.3f);
				action_color_gray_active = LERP_GRAY(0.35f);

				highlited_window_bg = LERP_GRAY(0.17f);
				main_titlebar_color = LERP_MAIN_COLOR_DARK(.5f);

				colors[ImGuiCol_Text] = LERP_GRAY(1.f);
				colors[ImGuiCol_TextDisabled] = LERP_GRAY(.7f);
				colors[ImGuiCol_WindowBg] = LERP_GRAY(.14f);
				colors[ImGuiCol_ChildBg] = LERP_GRAY(.14f);
				colors[ImGuiCol_PopupBg] = LERP_GRAY(.12f);
				colors[ImGuiCol_Border] = LERP_GRAY_A(.43f, .5f);
				colors[ImGuiCol_BorderShadow] = LERP_GRAY_A(.12f, .5f);
				colors[ImGuiCol_FrameBg] = LERP_GRAY_A(.06f, .54f);
				colors[ImGuiCol_FrameBgHovered] = LERP_GRAY_A(.19f, .4f);
				colors[ImGuiCol_FrameBgActive] = LERP_GRAY_A(.3f, .67f);
				colors[ImGuiCol_TitleBg] = action_color_00_default;
				colors[ImGuiCol_TitleBgActive] = action_color_00_active;
				colors[ImGuiCol_TitleBgCollapsed] = action_color_00_default;
				colors[ImGuiCol_MenuBarBg] = LERP_GRAY(.1f);
				colors[ImGuiCol_ScrollbarBg] = LERP_GRAY(0.23f);
				colors[ImGuiCol_ScrollbarGrab] = action_color_00_default;
				colors[ImGuiCol_ScrollbarGrabHovered] = action_color_00_hover;
				colors[ImGuiCol_ScrollbarGrabActive] = action_color_00_active;
				colors[ImGuiCol_CheckMark] = action_color_00_active;
				colors[ImGuiCol_SliderGrab] = action_color_00_default;
				colors[ImGuiCol_SliderGrabActive] = action_color_00_active;
				colors[ImGuiCol_Button] = action_color_00_default;
				colors[ImGuiCol_ButtonHovered] = action_color_00_hover;
				colors[ImGuiCol_ButtonActive] = action_color_00_active;
				colors[ImGuiCol_Header] = LERP_GRAY(.3f);
				colors[ImGuiCol_HeaderHovered] = LERP_GRAY(.4f);
				colors[ImGuiCol_HeaderActive] = LERP_GRAY(.5f);
				colors[ImGuiCol_Separator] = LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorHovered] = LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorActive] = LERP_GRAY(.45f);
				colors[ImGuiCol_ResizeGrip] = action_color_00_default;
				colors[ImGuiCol_ResizeGripHovered] = action_color_00_hover;
				colors[ImGuiCol_ResizeGripActive] = action_color_00_active;
				colors[ImGuiCol_Tab] = action_color_00_default;
				colors[ImGuiCol_TabHovered] = action_color_00_hover;
				colors[ImGuiCol_TabActive] = action_color_00_active;
				colors[ImGuiCol_TabUnfocused] = LERP_MAIN_COLOR_DARK(0.5f);
				colors[ImGuiCol_TabUnfocusedActive] = LERP_MAIN_COLOR_DARK(0.6f);
				colors[ImGuiCol_DockingPreview] = action_color_00_active;
				colors[ImGuiCol_DockingEmptyBg] = LERP_GRAY(0.2f);
				colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = LERP_MAIN_COLOR_DARK(.75f);
				colors[ImGuiCol_PlotHistogramHovered] = action_color_00_active;
				colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
				colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
				colors[ImGuiCol_TextSelectedBg] = LERP_MAIN_COLOR_DARK(.4f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
				colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

			} break;

			case PFF::UI::THEME::theme_selection::light:
			{

				action_color_00_default = LERP_MAIN_COLOR_LIGHT(0.7f);
				action_color_00_hover = LERP_MAIN_COLOR_LIGHT(0.85f);
				action_color_00_active = LERP_MAIN_COLOR_LIGHT(1.f);

				action_color_gray_default = LERP_GRAY(0.2f);
				action_color_gray_hover = LERP_GRAY(0.27f);
				action_color_gray_active = LERP_GRAY(0.35f);

				highlited_window_bg = LERP_GRAY(0.8f);
				main_titlebar_color = LERP_MAIN_COLOR_LIGHT(.5f);

				colors[ImGuiCol_Text] = LERP_GRAY(.0f);
				colors[ImGuiCol_TextDisabled] = LERP_GRAY(.3f);
				colors[ImGuiCol_WindowBg] = LERP_GRAY(.85f);
				colors[ImGuiCol_ChildBg] = LERP_GRAY(.85f);
				colors[ImGuiCol_PopupBg] = LERP_GRAY(.8f);
				colors[ImGuiCol_Border] = LERP_GRAY_A(0.2f, .50f);
				colors[ImGuiCol_BorderShadow] = LERP_GRAY(.12f);
				colors[ImGuiCol_FrameBg] = LERP_GRAY_A(.75f, .75f);
				colors[ImGuiCol_FrameBgHovered] = LERP_GRAY_A(.70f, .75f);
				colors[ImGuiCol_FrameBgActive] = LERP_GRAY_A(.65f, .75f);
				colors[ImGuiCol_TitleBg] = action_color_00_default;
				colors[ImGuiCol_TitleBgActive] = action_color_00_active;
				colors[ImGuiCol_TitleBgCollapsed] = action_color_00_default;
				colors[ImGuiCol_MenuBarBg] = LERP_GRAY(.58f);
				colors[ImGuiCol_ScrollbarBg] = LERP_GRAY(.75f);
				colors[ImGuiCol_ScrollbarGrab] = action_color_00_default;
				colors[ImGuiCol_ScrollbarGrabHovered] = action_color_00_hover;
				colors[ImGuiCol_ScrollbarGrabActive] = action_color_00_active;
				colors[ImGuiCol_CheckMark] = action_color_00_active;
				colors[ImGuiCol_SliderGrab] = action_color_00_default;
				colors[ImGuiCol_SliderGrabActive] = action_color_00_active;
				colors[ImGuiCol_Button] = action_color_00_default;
				colors[ImGuiCol_ButtonHovered] = action_color_00_hover;
				colors[ImGuiCol_ButtonActive] = action_color_00_active;
				colors[ImGuiCol_Header] = LERP_GRAY(.75f);
				colors[ImGuiCol_HeaderHovered] = LERP_GRAY(.7f);
				colors[ImGuiCol_HeaderActive] = LERP_GRAY(.65f);
				colors[ImGuiCol_Separator] = LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorHovered] = LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorActive] = LERP_GRAY(.45f);
				colors[ImGuiCol_ResizeGrip] = action_color_00_default;
				colors[ImGuiCol_ResizeGripHovered] = action_color_00_hover;
				colors[ImGuiCol_ResizeGripActive] = action_color_00_active;
				colors[ImGuiCol_Tab] = action_color_00_default;
				colors[ImGuiCol_TabHovered] = action_color_00_hover;
				colors[ImGuiCol_TabActive] = action_color_00_active;
				colors[ImGuiCol_TabUnfocused] = LERP_MAIN_COLOR_LIGHT(0.5f);
				colors[ImGuiCol_TabUnfocusedActive] = LERP_MAIN_COLOR_LIGHT(0.6f);
				colors[ImGuiCol_DockingPreview] = action_color_00_active;
				colors[ImGuiCol_DockingEmptyBg] = LERP_GRAY(.2f);
				colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = LERP_MAIN_COLOR_LIGHT(.75f);
				colors[ImGuiCol_PlotHistogramHovered] = action_color_00_active;
				colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
				colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
				colors[ImGuiCol_TextSelectedBg] = LERP_MAIN_COLOR_LIGHT(.4f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
				colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

			} break;

			default:
				break;
			}

			serialize_data(serializer::option::load_from_file);
		}

		void PFF_API update_UI_colors(ImVec4 new_color) {

			main_color = new_color;
			serialize_data(serializer::option::save_to_file);
			update_UI_theme();
		}

#endif // !UI_THEME_IMPLEMENTATION

	}
}
