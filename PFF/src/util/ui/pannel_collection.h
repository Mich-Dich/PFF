#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

#include "application.h"


namespace PFF::UI {

	enum class window_pos {

		center = 0,
		custom = 1,
		top_left = 2,
		top_right = 3,
		bottom_left = 4,
		bottom_right = 5,
	};


	PFF_API void set_next_window_pos(window_pos location, f32 padding = 10.f);

	PFF_API void set_next_window_pos_in_window(window_pos location, f32 padding = 10.f);

	PFF_API void next_window_position_selector(window_pos& position, bool& show_window);


	// @brief Draws a vertical separation line.
	PFF_API void seperation_vertical();

	PFF_API bool add_gray_button(const char* lable, const ImVec2& size = { 0, 0 });

	// @brief Draws text using a larger font.
	// @param [text] The text to be drawn.
	PFF_API void big_text(const char* text, bool wrapped = false);

	// @brief Displays a help marker with tooltip containing the provided description.
	// @param [desc] The description text to be displayed in the tooltip.
	PFF_API void help_marker(const char* desc);

	// @brief Adjusts the current ImGui cursor position by adding the specified horizontal and vertical shift offsets.
	// @param [shift_x] The horizontal shift offset.
	// @param [shift_y] The vertical shift offset.
	PFF_API void shift_cursor_pos(const f32 shift_x, const f32 shift_y);

	PFF_API void progressbar_with_text(const char* lable, const char* progress_bar_text, f32 percent, f32 lable_size = 50.f, f32 progressbar_size_x = 50.f, f32 progressbar_size_y = 1.f);

	// @brief This function sets up an ImGui table with two columns, where the first column is resizable and the second column fills the remaining available area
	// @brief CAUTION - you need to call UI::end_table() at the end of the table;
	// @param [lable] Is used to identify the table
	PFF_API void begin_table(std::string_view lable, bool display_name = true, ImVec2 size = ImVec2(0,0) );

	// @brief Ends the table started with UI::begin_table().
	PFF_API void end_table();

	// @brief This function draws a custom frame with two separate sections: [left_side] and [right_side].
	//          The width of the first column is specified by [width_left_side]. Both sections are contained within
	//          the same ImGui table. Each section's content is drawn using the provided function callbacks (lamdas or functions)
	// @param [width_left_side] The fixed width of the first column.
	// @param [left_side] The function representing the content of the left side.
	// @param [right_side] The function representing the content of the right side.
	PFF_API void custom_frame(const f32 width_left_side, std::function<void()> left_side, std::function<void()> right_side);

	
	// @brief Renders an integer slider within a table row in an ImGui interface.
	// 
	// This function creates a row in an ImGui table, sets the label in the first column,
	// and places an integer slider in the second column. The slider modifies the provided
	// integer value within the specified range.
	// 
	// @param label The text label for the slider, displayed in the first column of the table.
	// @param value A reference to the integer value to be modified by the slider.
	// @param min_value The minimum value for the slider.
	// @param max_value The maximum value for the slider.
	// @param flags Optional ImGui input text flags.
	// 
	// @return true if the value was changed by the slider, false otherwise.
	PFF_API bool table_row_slider(std::string_view label, int& value, int min_value = 0, int max_value = 1, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);


	// @brief Add
	PFF_API void table_row(std::function<void()> first_colum, std::function<void()> second_colum);


	// @brief Renders a slider within a table row in an ImGui interface.
	// 
	// This function creates a row in an ImGui table, sets the label in the first column,
	// and places a slider in the second column. The type of the slider is determined by
	// the type of the value parameter.
	// 
	// @tparam T The type of the value to be modified by the slider. Supported types are:
	// @tparam      - int
	// @tparam      - f32 (float)
	// @tparam      - f64 (double)
	// @tparam      - glm::vec2
	// @tparam      - glm::vec3
	// @tparam      - glm::vec4
	// @tparam      - ImVec2
	// @tparam      - ImVec4
	// 
	// @param label The text label for the slider, displayed in the first column of the table.
	// @param value A reference to the value to be modified by the slider.
	// @param min_value The minimum value for the slider. Defaults to 0.f.
	// @param max_value The maximum value for the slider. Defaults to 1.f.
	// @param flags Optional ImGui input text flags. Defaults to ImGuiInputTextFlags_None.
	// 
	// @return true if the value was changed by the slider, false otherwise.
	template<typename T>
	bool table_row_slider(std::string_view label, T& value, f32 min_value = 0.f, f32 max_value = 1.f, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None) {

		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 current_item_spacing = style.ItemSpacing;
		flags |= ImGuiInputTextFlags_AllowTabInput;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);

		// Copy non-space characters from label to loc_label
		std::string loc_label = "##";
		loc_label.reserve(label.size() + 2);
		std::remove_copy_if(label.begin(), label.end(), std::back_inserter(loc_label), [](char c) {
			return std::isspace(static_cast<unsigned char>(c));
		});

		ImGui::SetNextItemWidth(ImGui::GetColumnWidth());

		if constexpr (std::is_same_v<T, int>)
			return ImGui::SliderInt(loc_label.c_str(), &value, static_cast<int>(min_value), static_cast<int>(max_value), "%d", flags);

		if constexpr (std::is_same_v<T, f32> || std::is_same_v<T, f64>)
			return ImGui::SliderFloat(loc_label.c_str(), &value, min_value, max_value, "%.2f", flags);

		if constexpr (std::is_same_v<T, glm::vec2> || std::is_same_v<T, ImVec2>)
			return ImGui::SliderFloat2(loc_label.c_str(), &value[0], min_value, max_value, "%.2f", flags);

		if constexpr (std::is_same_v<T, glm::vec3>)
			return ImGui::SliderFloat3(loc_label.c_str(), &value[0], min_value, max_value, "%.2f", flags);

		if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>)
			return ImGui::SliderFloat4(loc_label.c_str(), &value[0], min_value, max_value, "%.2f", flags);

		return false;
	}

	// @brief Adds a row to an ImGui table with a label and corresponding value input field.
	// @tparam [T] The type of the value.
	// @param [label] The label for the row.
	// @param [value] The value to be displayed or edited.
	// @param [flags] Flags controlling the behavior of the input field.
	template<typename T>
	bool table_row(std::string_view label, T& value, f32 drag_speed = 0.01f, f32 min_value = 0.f, f32 max_value = 0.f, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None) {

		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 current_item_spacing = style.ItemSpacing;
		flags |= ImGuiInputTextFlags_AllowTabInput;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);

		std::string loc_lable = "##" + *label.data();

		if constexpr (std::is_same_v<T, bool>) {

			ImGui::Text("%s", util::bool_to_str(value));
			return false;
		}

		else if constexpr (std::is_arithmetic_v<T>) {

			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			return ImGui::DragFloat(loc_lable.c_str(), &value, drag_speed, min_value, max_value, "%.2f", flags);
		}

		else if constexpr (std::is_same_v<T, glm::vec2> || std::is_same_v<T, ImVec2>) {

			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			return ImGui::DragFloat2(loc_lable.c_str(), &value[0], drag_speed, min_value, max_value, "%.2f", flags);
		}

		else if constexpr (std::is_same_v<T, glm::vec3>) {

			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			return ImGui::DragFloat3(loc_lable.c_str(), &value[0], drag_speed, min_value, max_value, "%.2f", flags);
		}

		else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>) {

			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			return ImGui::DragFloat4(loc_lable.c_str(), &value[0], drag_speed, min_value, max_value, "%.2f", flags);
		}

		else if constexpr (std::is_convertible_v<T, std::string>) {

			ImGui::Text("%s", std::to_string(value).c_str());
			return false;
		}

		return false;
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