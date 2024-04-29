#pragma once

#include "util/pffpch.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>

#include "application.h"


namespace PFF::UI {

	void seperation_vertical();
	void help_marker(const char* desc);
	void shift_cursor_pos(const f32 shift_x = 0.0f, const f32 shift_y = 0.0f);

	// ----------------------------------------- static funcs -----------------------------------------

	// @brief Draws a vertical separation line.
	static void seperation_vertical() {

		ImGui::SameLine();
		shift_cursor_pos(5, 0);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		shift_cursor_pos(5, 0);
	}

	static bool add_gray_button(const char* lable, const ImVec2& size = { 0, 0 }) {

		ImGui::PushStyleColor(ImGuiCol_Button, UI::action_color_gray_default);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::action_color_gray_hover);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::action_color_gray_active);

		const bool result = ImGui::Button(lable, size);

		ImGui::PopStyleColor(3);
		return result;
	}

	// @brief Draws text using a larger font.
	// @param [text] The text to be drawn.
	static void draw_big_text(const char* text, bool wrapped = false) {

		//ImGui::PushFont(application::get().get_imgui_layer()->get_font("regular_big"));

		if(wrapped)
			ImGui::TextWrapped(text);
		else
			ImGui::Text(text);

		ImGui::PopFont();
	}

	// @brief Displays a help marker with tooltip containing the provided description.
	// @param [desc] The description text to be displayed in the tooltip.
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

	// @brief Adjusts the current ImGui cursor position by adding the specified horizontal and vertical shift offsets.
	// @param [shift_x] The horizontal shift offset.
	// @param [shift_y] The vertical shift offset.
	static void shift_cursor_pos(const f32 shift_x, const f32 shift_y) {

		auto current_pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos({ current_pos.x + shift_x, current_pos.y + shift_y });
	}

	// @brief Adds a row to an ImGui table with a label and corresponding value input field.
	// @tparam [T] The type of the value.
	// @param [label] The label for the row.
	// @param [value] The value to be displayed or edited.
	// @param [flags] Flags controlling the behavior of the input field.
	template<typename T>
	void add_table_row(std::string_view label, T& value, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None, f32 drag_speed = 0.01f, f32 min_value = 0.f, f32 max_value = 0.f) {

		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 current_item_spacing = style.ItemSpacing;
		flags |= ImGuiInputTextFlags_AllowTabInput;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);

		if constexpr (std::is_same_v<T, bool>) {

			ImGui::Text("%s", util::bool_to_str(value));
		}

		else if constexpr (std::is_arithmetic_v<T>) {

			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			ImGui::DragFloat(label.data(), &value, drag_speed, min_value, max_value, "%.2f", flags);
		}

		else if constexpr (std::is_same_v<T, glm::vec2> || std::is_same_v<T, ImVec2>) {

			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			ImGui::DragFloat2(label.data(), &value[0], drag_speed, min_value, max_value, "%.2f", flags);
		}

		else if constexpr (std::is_same_v<T, glm::vec3>) {

			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			ImGui::DragFloat3(label.data(), &value[0], drag_speed, min_value, max_value, "%.2f", flags);
		}

		else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>) {

			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			ImGui::DragFloat4(label.data(), &value[0], drag_speed, min_value, max_value, "%.2f", flags);
		}

		else if constexpr (std::is_convertible_v<T, std::string>) {

			ImGui::Text("%s", std::to_string(value).c_str());
		}
	}

	// @brief This function sets up an ImGui table with two columns, where the first column is resizable and the second column fills the remaining available area and not resizable
	// @brief CAUTION - you slao need to call UI::end_default_table() at the end of table;
	// @param [lable] Is used to identify the table
	static void begin_default_table(std::string_view lable, bool display_name = true) {

		if (display_name)
			ImGui::Text("%s:", lable.data());

		// setup table and columns
		ImGuiTableFlags flags = ImGuiTableFlags_Resizable;
		ImGui::BeginTable(lable.data(), 2, flags);
		ImGui::TableSetupColumn("##one", ImGuiTableColumnFlags_NoHeaderLabel);
		ImGui::TableSetupColumn("##two", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_NoResize);
	}

	// @brief Ends the table started with UI::begin_default_table("example").
	static void end_default_table() {

		ImGui::EndTable();
	}

	// @brief This function draws a custom frame with two separate sections: [left_side] and [right_side].
	//          The width of the first column is specified by [width_left_side]. Both sections are contained within
	//          the same ImGui table. Each section's content is drawn using the provided function callbacks (lamdas or functions)
	// @param [width_left_side] The fixed width of the first column.
	// @param [left_side] The function representing the content of the left side.
	// @param [right_side] The function representing the content of the right side.
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
			const ImVec4 color = UI::highlited_window_bg;
			const ImVec2 uperleft_corner = ImGui::GetCursorScreenPos();
			const ImVec2 lowerright_corner = { uperleft_corner.x + width_left_side ,uperleft_corner.y + ImGui::GetWindowHeight() };
			ImGui::GetWindowDrawList()->AddRectFilled(uperleft_corner, lowerright_corner, PFF::UI::convert_color_to_int(color));

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


// ImGuiSliderFlags_Logarithmic
// ImGuiSliderFlags_NoInput

/*
// Flags for ImGui::InputText()
// (Those are per-item flags. There are shared flags in ImGuiIO: io.ConfigInputTextCursorBlink and io.ConfigInputTextEnterKeepActive)
enum ImGuiInputTextFlags_ {
	ImGuiInputTextFlags_None = 0,
	ImGuiInputTextFlags_CharsDecimal = 1 << 0,   // Allow 0123456789.+-
	ImGuiInputTextFlags_CharsHexadecimal = 1 << 1,   // Allow 0123456789ABCDEFabcdef
	ImGuiInputTextFlags_CharsUppercase = 1 << 2,   // Turn a..z into A..Z
	ImGuiInputTextFlags_CharsNoBlank = 1 << 3,   // Filter out spaces, tabs
	ImGuiInputTextFlags_AutoSelectAll = 1 << 4,   // Select entire text when first taking mouse focus
	ImGuiInputTextFlags_EnterReturnsTrue = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
	ImGuiInputTextFlags_CallbackCompletion = 1 << 6,   // Callback on pressing TAB (for completion handling)
	ImGuiInputTextFlags_CallbackHistory = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
	ImGuiInputTextFlags_CallbackAlways = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
	ImGuiInputTextFlags_CallbackCharFilter = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
	ImGuiInputTextFlags_AllowTabInput = 1 << 10,  // Pressing TAB input a '\t' character into the text field
	ImGuiInputTextFlags_CtrlEnterForNewLine = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
	ImGuiInputTextFlags_NoHorizontalScroll = 1 << 12,  // Disable following the cursor horizontally
	ImGuiInputTextFlags_AlwaysOverwrite = 1 << 13,  // Overwrite mode
	ImGuiInputTextFlags_ReadOnly = 1 << 14,  // Read-only mode
	ImGuiInputTextFlags_Password = 1 << 15,  // Password mode, display all characters as '*'
	ImGuiInputTextFlags_NoUndoRedo = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
	ImGuiInputTextFlags_CharsScientific = 1 << 17,  // Allow 0123456789.+- eE (Scientific notation input)
	ImGuiInputTextFlags_CallbackResize = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)
	ImGuiInputTextFlags_CallbackEdit = 1 << 19,  // Callback on any edit (note that InputText() already returns true on edit, the callback is useful mainly to manipulate the underlying buffer while focus is active)
	ImGuiInputTextFlags_EscapeClearsAll = 1 << 20,  // Escape key clears content if not empty, and deactivate otherwise (contrast to default behavior of Escape to revert)

// Obsolete names
//ImGuiInputTextFlags_AlwaysInsertMode  = ImGuiInputTextFlags_AlwaysOverwrite   // [renamed in 1.82] name was not matching behavior
}; */