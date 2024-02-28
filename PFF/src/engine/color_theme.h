#pragma once

#include "util/pffpch.h"
#include <imgui.h>


namespace PFF::UI::THEME {

#define PFF_UI_ACTIVE_THEME				PFF::UI::THEME::current_theme

#define LERP_MAIN_COLOR(value)			{main_color.x * value, main_color.y * value, main_color.z * value, 1.f }		// Set [w] to be [1.f] to disable accidental transparency
#define LERP_GRAY(value)				{value, value, value, 1.f }
#define LERP_GRAY_A(value, alpha)		{value, value, value, alpha }


	enum class theme_selection {
		default,
		light
	};

	static theme_selection UI_theme = theme_selection::default;
	static ImVec4 main_color = {.5f,	.0f,	.0f,	1.f };

	void enable_window_border(bool enable) {

		bool loc_enable = enable;
		config::save(config::file::editor, "UI", "window_border", loc_enable);

		ImGuiStyle* style = &ImGui::GetStyle();
		style->WindowBorderSize = loc_enable ? 1.f : 0.f;
	}

	void update_UI_theme() {

		config::save(config::file::editor, "UI", "theme", UI::THEME::UI_theme);

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		// main sizes
		style->WindowPadding			= ImVec2(10.f, 4.f);
		style->FramePadding				= ImVec2(4.f, 4.f);
		style->CellPadding				= ImVec2(4.f, 4.f);
		style->ItemSpacing				= ImVec2(4.f, 4.f);
		style->ItemInnerSpacing			= ImVec2(4.f, 4.f);
		style->TouchExtraPadding		= ImVec2(4.f, 4.f);
		style->IndentSpacing			= 10.f;
		style->ScrollbarSize			= 14.f;
		style->GrabMinSize				= 14.f;

		// border
		style->WindowBorderSize			= 1.0f;
		style->ChildBorderSize			= 0.0f;
		style->PopupBorderSize			= 0.0f;
		style->FrameBorderSize			= 0.0f;
		style->TabBorderSize			= 0.0f;

		// padding
		style->WindowRounding			= 2.f;
		style->ChildRounding			= 2.f;
		style->FrameRounding			= 2.f;
		style->PopupRounding			= 2.f;
		style->ScrollbarRounding		= 2.f;
		style->GrabRounding				= 2.f;
		style->TabRounding				= 2.f;

		switch (UI_theme) {
			case PFF::UI::THEME::theme_selection::default: {

				colors[ImGuiCol_Text]							= LERP_GRAY(1.f);
				colors[ImGuiCol_TextDisabled]					= LERP_GRAY(.7f);
				colors[ImGuiCol_WindowBg]						= LERP_GRAY(.14f);
				colors[ImGuiCol_ChildBg]						= LERP_GRAY(.14f);
				colors[ImGuiCol_PopupBg]						= LERP_GRAY(.12f);
				colors[ImGuiCol_Border]							= LERP_GRAY_A(.43f, .5f);
				colors[ImGuiCol_BorderShadow]					= LERP_GRAY_A(.12f, .5f);
				colors[ImGuiCol_FrameBg]						= LERP_GRAY_A(.06f, .54f);
				colors[ImGuiCol_FrameBgHovered]					= LERP_GRAY_A(.19f, .4f);
				colors[ImGuiCol_FrameBgActive]					= LERP_GRAY_A(.3f, .67f);
				colors[ImGuiCol_TitleBg]						= LERP_MAIN_COLOR(.7f);
				colors[ImGuiCol_TitleBgActive]					= LERP_MAIN_COLOR(1.f);
				colors[ImGuiCol_TitleBgCollapsed]				= LERP_MAIN_COLOR(.7f);
				colors[ImGuiCol_MenuBarBg]						= LERP_GRAY(.1f);
				colors[ImGuiCol_ScrollbarBg]					= LERP_GRAY(0.23f);
				colors[ImGuiCol_ScrollbarGrab]					= LERP_MAIN_COLOR(.7f);
				colors[ImGuiCol_ScrollbarGrabHovered]			= LERP_MAIN_COLOR(.85f);
				colors[ImGuiCol_ScrollbarGrabActive]			= LERP_MAIN_COLOR(1.f);
				colors[ImGuiCol_CheckMark]						= LERP_MAIN_COLOR(1.f);
				colors[ImGuiCol_SliderGrab]						= LERP_MAIN_COLOR(.7f);
				colors[ImGuiCol_SliderGrabActive]				= LERP_MAIN_COLOR(1.f);
				colors[ImGuiCol_Button]							= LERP_MAIN_COLOR(.7f);
				colors[ImGuiCol_ButtonHovered]					= LERP_MAIN_COLOR(.85f);
				colors[ImGuiCol_ButtonActive]					= LERP_MAIN_COLOR(1.f);
				colors[ImGuiCol_Header]							= LERP_GRAY(.3f);
				colors[ImGuiCol_HeaderHovered]					= LERP_GRAY(.4f);
				colors[ImGuiCol_HeaderActive]					= LERP_GRAY(.5f);
				colors[ImGuiCol_Separator]						= LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorHovered]				= LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorActive]				= LERP_GRAY(.45f);
				colors[ImGuiCol_ResizeGrip]						= LERP_MAIN_COLOR(0.7f);
				colors[ImGuiCol_ResizeGripHovered]				= LERP_MAIN_COLOR(0.85f);
				colors[ImGuiCol_ResizeGripActive]				= LERP_MAIN_COLOR(1.f);
				colors[ImGuiCol_Tab]							= LERP_MAIN_COLOR(0.7f);
				colors[ImGuiCol_TabHovered]						= LERP_MAIN_COLOR(0.85f);
				colors[ImGuiCol_TabActive]						= LERP_MAIN_COLOR(1.f);
				colors[ImGuiCol_TabUnfocused]					= LERP_MAIN_COLOR(0.5f);
				colors[ImGuiCol_TabUnfocusedActive]				= LERP_MAIN_COLOR(0.6f);
				colors[ImGuiCol_DockingPreview]					= LERP_MAIN_COLOR(1.f);
				colors[ImGuiCol_DockingEmptyBg]					= LERP_GRAY(0.2f);
				colors[ImGuiCol_PlotLines]						= ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered]				= ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
				colors[ImGuiCol_PlotHistogram]					= LERP_MAIN_COLOR(.75f);
				colors[ImGuiCol_PlotHistogramHovered]			= LERP_MAIN_COLOR(1.f);
				colors[ImGuiCol_TableHeaderBg]					= ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
				colors[ImGuiCol_TableBorderStrong]				= ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableBorderLight]				= ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableRowBg]						= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_TableRowBgAlt]					= ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
				colors[ImGuiCol_TextSelectedBg]					= LERP_MAIN_COLOR(.4f);
				colors[ImGuiCol_DragDropTarget]					= ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
				colors[ImGuiCol_NavHighlight]					= ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight]			= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg]				= ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg]				= ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

			} break;

			case PFF::UI::THEME::theme_selection::light: {

				colors[ImGuiCol_Text]							= LERP_GRAY(.0f);
				colors[ImGuiCol_TextDisabled]					= LERP_GRAY(.3f);
				colors[ImGuiCol_WindowBg]						= LERP_GRAY(.85f);
				colors[ImGuiCol_ChildBg]						= LERP_GRAY(.85f);
				colors[ImGuiCol_PopupBg]						= LERP_GRAY(.8f);
				colors[ImGuiCol_Border]							= LERP_GRAY_A(0.2f, .50f);
				colors[ImGuiCol_BorderShadow]					= LERP_GRAY(.12f);
				colors[ImGuiCol_FrameBg]						= LERP_GRAY_A(.75f, .75f);
				colors[ImGuiCol_FrameBgHovered]					= LERP_GRAY_A(.70f, .75f);
				colors[ImGuiCol_FrameBgActive]					= LERP_GRAY_A(.65f, .75f);
				colors[ImGuiCol_TitleBg]						= LERP_MAIN_COLOR(.45f);
				colors[ImGuiCol_TitleBgActive]					= LERP_MAIN_COLOR(.55f);
				colors[ImGuiCol_TitleBgCollapsed]				= LERP_MAIN_COLOR(.45f);
				colors[ImGuiCol_MenuBarBg]						= LERP_GRAY(.58f);
				colors[ImGuiCol_ScrollbarBg]					= LERP_GRAY(.75f);
				colors[ImGuiCol_ScrollbarGrab]					= LERP_MAIN_COLOR(.5f);
				colors[ImGuiCol_ScrollbarGrabHovered]			= LERP_MAIN_COLOR(.7f);
				colors[ImGuiCol_ScrollbarGrabActive]			= LERP_MAIN_COLOR(.8f);
				colors[ImGuiCol_CheckMark]						= LERP_MAIN_COLOR(.7f);
				colors[ImGuiCol_SliderGrab]						= LERP_MAIN_COLOR(.7f);
				colors[ImGuiCol_SliderGrabActive]				= LERP_MAIN_COLOR(.8f);
				colors[ImGuiCol_Button]							= LERP_MAIN_COLOR(.7f);
				colors[ImGuiCol_ButtonHovered]					= LERP_MAIN_COLOR(.8f);
				colors[ImGuiCol_ButtonActive]					= LERP_MAIN_COLOR(.9f);
				colors[ImGuiCol_Header]							= LERP_GRAY(.75f);
				colors[ImGuiCol_HeaderHovered]					= LERP_GRAY(.7f);
				colors[ImGuiCol_HeaderActive]					= LERP_GRAY(.65f);
				colors[ImGuiCol_Separator]						= LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorHovered]				= LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorActive]				= LERP_GRAY(.45f);
				colors[ImGuiCol_ResizeGrip]						= LERP_MAIN_COLOR(.5f);
				colors[ImGuiCol_ResizeGripHovered]				= LERP_MAIN_COLOR(.7f);
				colors[ImGuiCol_ResizeGripActive]				= LERP_MAIN_COLOR(.8f);
				colors[ImGuiCol_Tab]							= LERP_MAIN_COLOR(.65f);
				colors[ImGuiCol_TabHovered]						= LERP_MAIN_COLOR(.75f);
				colors[ImGuiCol_TabActive]						= LERP_MAIN_COLOR(.6f);
				colors[ImGuiCol_TabUnfocused]					= LERP_MAIN_COLOR(0.5f);
				colors[ImGuiCol_TabUnfocusedActive]				= LERP_MAIN_COLOR(0.6f);
				colors[ImGuiCol_DockingPreview]					= LERP_MAIN_COLOR(.4f);
				colors[ImGuiCol_DockingEmptyBg]					= LERP_GRAY(.2f);
				colors[ImGuiCol_PlotLines]						= ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered]				= ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
				colors[ImGuiCol_PlotHistogram]					= LERP_MAIN_COLOR(.75f);
				colors[ImGuiCol_PlotHistogramHovered]			= LERP_MAIN_COLOR(1.f);
				colors[ImGuiCol_TableHeaderBg]					= ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
				colors[ImGuiCol_TableBorderStrong]				= ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableBorderLight]				= ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableRowBg]						= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_TableRowBgAlt]					= ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
				colors[ImGuiCol_TextSelectedBg]					= LERP_MAIN_COLOR(.8f);
				colors[ImGuiCol_DragDropTarget]					= ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
				colors[ImGuiCol_NavHighlight]					= ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight]			= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg]				= ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg]				= ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

			} break;

			default:
				break;
		}
	}

	void update_UI_colors(ImVec4 new_color) {

		main_color = new_color;
		config::save(config::file::editor, "UI", "main_color", UI::THEME::main_color);
		update_UI_theme();
	}

}
