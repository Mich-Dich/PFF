
#include <util/pch_editor.h>
/*

#include <imgui.h>
#include <imgui_internal.h>
//#include <imgui_impl_glfw.h>		// TODO: remove
//#include <imgui_impl_vulkan.h>	// TODO: remove
#include <glm/glm.hpp>

#include "engine/color_theme.h"

#include "pannel_collection.h"

namespace PFF::UI {
	
		static void seperation_vertical() {

		ImGui::SameLine();
		shift_cursor_pos(5, 0);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		shift_cursor_pos(5, 0);
	}

	static void draw_big_text(const char* text) {

		ImGui::PushFont(application::get().get_imgui_layer()->get_font("regular_big"));
		ImGui::Text(text);
		ImGui::PopFont();
	}

	static void help_marker(const char* desc) {

		ImGui::TextDisabled(" ? ");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	static void shift_cursor_pos(const f32 shift_x, const f32 shift_y) {

		auto current_pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos({ current_pos.x + shift_x, current_pos.y + shift_y });
	}

	static void begin_default_table(const char* lable) {

		// setup table
		ImGuiTableFlags flags = ImGuiTableFlags_Resizable;
		ImGui::BeginTable(lable, 2, flags);

		// setup column
		ImGui::TableSetupColumn("##one", ImGuiTableColumnFlags_NoHeaderLabel);
		ImGui::TableSetupColumn("##two", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_NoResize);
	}

	static void end_default_table() {

		ImGui::EndTable();
	}

	static void custom_frame(const f32 width_left_side, std::function<void()> left_side, std::function<void()> right_side) {

		// SETUP
		ImGuiTableFlags flags = ImGuiTableFlags_None;
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
		if (ImGui::BeginTable("test_master_divider", 2, flags)) {

			ImGui::PopStyleVar();

			// setup column
			ImGui::TableSetupColumn("##one", ImGuiTableColumnFlags_WidthFixed, width_left_side);
			ImGui::TableSetupColumn("##two", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, ImGui::GetWindowWidth() - (width_left_side + 10.f));

			// enter first column
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			// draw background
			const ImVec4 color = UI::THEME::highlited_window_bg;
			const ImVec2 uperleft_corner = ImGui::GetCursorScreenPos();
			const ImVec2 lowerright_corner = { uperleft_corner.x + width_left_side ,uperleft_corner.y + ImGui::GetWindowHeight() };
			ImGui::GetWindowDrawList()->AddRectFilled(uperleft_corner, lowerright_corner, PFF::convert_color_to_int(color));

			shift_cursor_pos(0, ImGui::GetTextLineHeight());
			ImGui::BeginGroup();
			left_side();
			ImGui::EndGroup();

			ImGui::TableSetColumnIndex(1);
			shift_cursor_pos(10, 10);

			ImGui::BeginGroup();
			right_side();
			ImGui::EndGroup();

			ImGui::EndTable();
		}
	}

	
}

*/