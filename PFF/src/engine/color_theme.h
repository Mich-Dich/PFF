#pragma once

#include "util/pffpch.h"
#include <imgui.h>

namespace PFF::UI::THEME {

	struct theme {
		ImVec4 main_bg_color;
		ImVec4 color_00_default;
		ImVec4 color_00_hover;
		ImVec4 color_00_active;
		ImVec4 color_01_default;
		ImVec4 color_01_hover;
		ImVec4 color_01_active;
		ImVec4 color_02_default;
		ImVec4 color_02_hover;
		ImVec4 color_02_active;
		ImVec4 color_backbround;
	};

	theme theme_default{
		ImVec4(0.8f, 0.8f, 0.8f, 1.0f),			// main_bg_color
		ImVec4(0.0f, 0.25f, 0.0f, 1.0f),
		ImVec4(0.0f, 0.36f, 0.0f, 1.0f),
		ImVec4(0.0f, 0.32f, 0.0f, 1.0f),
		ImVec4(0.0f, 0.2f, 0.0f, 1.0f),
		ImVec4(0.0f, 0.38f, 0.0f, 1.0f),
		ImVec4(0.0f, 0.29f, 0.0f, 1.0f),
		ImVec4(0.0f, 0.35f, 0.0f, 1.0f),
		ImVec4(0.0f, 0.75f, 0.0f, 1.0f),
		ImVec4(0.0f, 0.6f, 0.0f, 1.0f),
		ImVec4(0.058f, 0.058f, 0.058f, 1.0f),

	};

	theme theme_light{
		ImVec4(0.058f, 0.058f, 0.058f, 1.0f),	// main_bg_color
		ImVec4(1.0f, 0.75f, 1.0f, 1.0f),		// Bright pink
		ImVec4(1.0f, 0.64f, 1.0f, 1.0f),		// Light purple
		ImVec4(1.0f, 0.68f, 1.0f, 1.0f),		// Light pink
		ImVec4(1.0f, 0.8f, 1.0f, 1.0f),			// Light green
		ImVec4(1.0f, 0.62f, 1.0f, 1.0f),		// Pale violet
		ImVec4(1.0f, 0.71f, 1.0f, 1.0f),		// Light lavender
		ImVec4(1.0f, 0.65f, 1.0f, 1.0f),		// Light purple
		ImVec4(1.0f, 0.25f, 1.0f, 1.0f),		// Light green
		ImVec4(1.0f, 0.4f, 1.0f, 1.0f),			// Light green
		ImVec4(0.942f, 0.942f, 0.942f, 1.0f)	// Light gray
	};

	static theme* current_theme = &theme_default;

	// PFF_API_EDITOR theme* get_active_theme() { return current_theme; }

}
