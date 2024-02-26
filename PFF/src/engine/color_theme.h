#pragma once

#include "util/pffpch.h"
#include <imgui.h>


namespace PFF::UI::THEME {

#define PFF_UI_ACTIVE_THEME			PFF::UI::THEME::current_theme

#define LERP_TARGET_COLOR(value)	ImVec4(target_color.x * value, target_color.y * value, target_color.z * value, target_color.w )
#define LERP_GRAY(value)			ImVec4(value, value, value, 1.f )

#define COLOR_BG					{0.14f,	0.14f,	0.14f,	1.f}

#define COLOR_0_DEFAULT				LERP_TARGET_COLOR(0.25f)
#define COLOR_0_HOVER				LERP_TARGET_COLOR(0.36f)
#define COLOR_0_ACTIVE				LERP_TARGET_COLOR(0.32f)

#define COLOR_1_DEFAULT				LERP_TARGET_COLOR(0.35f)
#define COLOR_1_HOVER				LERP_TARGET_COLOR(0.75f)
#define COLOR_1_ACTIVE				LERP_TARGET_COLOR(0.6f)

#define COLOR_BLUE_0_DEFAULT		{0.f,	0.25f,	0.f,	1.f}
#define COLOR_BLUE_0_HOVER			{0.f,	0.36f,	0.f,	1.f}
#define COLOR_BLUE_0_ACTIVE			{0.f,	0.32f,	0.f,	1.f}

#define COLOR_BLUE_1_DEFAULT		{0.f,	0.f,	0.35f,	1.f}
#define COLOR_BLUE_1_HOVER			{0.f,	0.f,	0.75f,	1.f}
#define COLOR_BLUE_1_ACTIVE			{0.f,	0.f,	0.6f,	1.f}

#define COLOR_GRAY_0_DEFAULT		{0.23f,	0.23f,	0.23f,	1.f}
#define COLOR_GRAY_0_HOVER			{0.34f,	0.34f,	0.34f,	1.f}
#define COLOR_GRAY_0_ACTIVE			{0.44f,	0.44f,	0.44f,	1.f}

	struct theme {

		ImVec4 Header;					// Header
		ImVec4 HeaderHovered;			// HeaderHovered
		ImVec4 HeaderActive;			// HeaderActive
		ImVec4 TabUnfocused;			// TabUnfocused
		ImVec4 TabUnfocusedActive;		// TabUnfocusedActive
		ImVec4 Button;					// Button
		ImVec4 ButtonHovered;			// ButtonHovered
		ImVec4 ButtonActive;			// ButtonActive
		ImVec4 Tab;						// Tab
		ImVec4 TabHovered;				// TabHovered
		ImVec4 TabActive;				// TabActive
		ImVec4 FrameBg;					// FrameBg
		ImVec4 FrameBgHovered;			// FrameBgHovered
		ImVec4 FrameBgActive;			// FrameBgActive
		ImVec4 ResizeGrip;				// ResizeGrip
		ImVec4 ResizeGripHovered;		// ResizeGripHovered
		ImVec4 ResizeGripActive;		// ResizeGripActive
		ImVec4 Separator;				// Separator
		ImVec4 SeparatorHovered;		// SeparatorHovered
		ImVec4 SeparatorActive;			// SeparatorActive
		ImVec4 TitleBg;					// TitleBg
		ImVec4 TitleBgActive;			// TitleBgActive
		ImVec4 TitleBgCollapsed;		// TitleBgCollapsed
		ImVec4 DockingPreview;			// DockingPreview
		ImVec4 MenuBarBg;				// MenuBarBg
		ImVec4 CheckMark;				// CheckMark
		ImVec4 SliderGrab;				// SliderGrab
		ImVec4 SliderGrabActive;		// SliderGrabActive
		ImVec4 ScrollbarBg;				// ScrollbarBg
		ImVec4 ScrollbarGrab;			// ScrollbarGrab
		ImVec4 ScrollbarGrabHovered;	// ScrollbarGrabHovered
		ImVec4 ScrollbarGrabActive;		// ScrollbarGrabActive
		ImVec4 TextSelectedBg;			// TextSelectedBg
		ImVec4 Border;					// Border
		ImVec4 WindowBg;				// WindowBg
		ImVec4 ChildBg;					// ChildBg
		ImVec4 PopupBg;					// PopupBg

	};

	static ImVec4 target_color = { 0.2f, .7f, 1.f, 1.f };		// TODO: expose to user with a submenu in MainMenuBar()

	static theme theme_default{

		COLOR_GRAY_0_DEFAULT,			// Header
		COLOR_GRAY_0_HOVER,				// HeaderHovered
		COLOR_GRAY_0_ACTIVE,			// HeaderActive
		COLOR_GRAY_0_DEFAULT,			// TabUnfocused
		COLOR_GRAY_0_HOVER,				// TabUnfocusedActive
		COLOR_0_DEFAULT,				// Button
		COLOR_0_HOVER,					// ButtonHovered
		COLOR_0_ACTIVE,					// ButtonActive
		COLOR_1_DEFAULT,				// Tab
		COLOR_1_HOVER,					// TabHovered
		COLOR_1_ACTIVE,					// TabActive
		LERP_TARGET_COLOR(.0f),			// FrameBg
		LERP_TARGET_COLOR(.4f),			// FrameBgHovered
		LERP_TARGET_COLOR(.3f),			// FrameBgActive
		COLOR_0_DEFAULT,				// ResizeGrip
		COLOR_0_HOVER,					// ResizeGripHovered
		COLOR_0_ACTIVE,					// ResizeGripActive
		LERP_GRAY(.45f),				// Separator
		LERP_GRAY(.45f),				// SeparatorHovered
		LERP_GRAY(.45f),				// SeparatorActive
		LERP_TARGET_COLOR(.25f),		// TitleBg
		LERP_TARGET_COLOR(.45f),		// TitleBgActive
		LERP_TARGET_COLOR(.1f),			// TitleBgCollapsed
		LERP_TARGET_COLOR(.4f),			// DockingPreview
		LERP_GRAY(.1f),					// MenuBarBg
		LERP_TARGET_COLOR(.625f),		// CheckMark
		COLOR_1_DEFAULT,				// SliderGrab
		COLOR_1_ACTIVE,					// SliderGrabActive
		COLOR_GRAY_0_DEFAULT,			// ScrollbarBg
		COLOR_1_DEFAULT,				// ScrollbarGrab
		COLOR_1_HOVER,					// ScrollbarGrabHovered
		COLOR_1_ACTIVE,					// ScrollbarGrabActive
		LERP_TARGET_COLOR(.2f),			// TextSelectedBg
		LERP_GRAY(0.f),					// Border
		COLOR_BG,						// WindowBg
		COLOR_BG,						// ChildBg
		LERP_GRAY(.12f),				// PopupBg

	};

	static theme theme_light{ };

	static theme* current_theme = &theme_default;

	// PFF_API_EDITOR theme* get_active_theme() { return current_theme; }

}
