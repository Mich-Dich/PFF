#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

#include "application.h"
#include "util/data_structures/UUID.h"


static FORCEINLINE ImVec2  operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static FORCEINLINE ImVec2  operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static FORCEINLINE ImVec2  operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static FORCEINLINE ImVec2  operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static FORCEINLINE ImVec2  operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static FORCEINLINE ImVec2  operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static FORCEINLINE ImVec2  operator-(const ImVec2& lhs) { return ImVec2(-lhs.x, -lhs.y); }
static FORCEINLINE ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static FORCEINLINE ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static FORCEINLINE ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static FORCEINLINE ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static FORCEINLINE ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static FORCEINLINE ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static FORCEINLINE bool    operator==(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
static FORCEINLINE bool    operator!=(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x != rhs.x || lhs.y != rhs.y; }
static FORCEINLINE ImVec4  operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static FORCEINLINE ImVec4  operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static FORCEINLINE ImVec4  operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
static FORCEINLINE bool    operator==(const ImVec4& lhs, const ImVec4& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w; }
static FORCEINLINE bool    operator!=(const ImVec4& lhs, const ImVec4& rhs) { return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w; }

namespace PFF::UI {

	enum class window_pos {

		center = 0,
		custom = 1,
		top_left = 2,
		top_right = 3,
		bottom_left = 4,
		bottom_right = 5,
	};

	enum class mouse_interation : u8 {

		none,
		hovered,						// The mouse cursor is over the item.
		left_clicked,					// The mouse button was pressed and released over the item.
		left_double_clicked,			// The mouse button was clicked twice in quick succession over the item.
		left_pressed,					// The mouse button is currently held down over the item.
		left_released,					// The mouse button was released over the item.
		right_clicked,					// The right mouse button was clicked over the item.
		right_double_clicked,			// The right mouse button was clicked over the item.
		right_pressed,					// The mouse button is currently held down over the item.
		right_released,					// The mouse button was released over the item.
		middle_clicked,					// The middle mouse button was clicked over the item.
		middle_double_clicked,			// The middle mouse button was clicked over the item.
		middle_pressed,					// The mouse button is currently held down over the item.
		middle_release,					// The mouse button is currently held down over the item.
		dragged,						// The item is being dragged with the mouse.
		focused,						// The item has keyboard focus(can be set with mouse clicks).
		active,							// The item is currently being interacted with
		deactivated,					// The item was previously active but is no longer being interacted with.
		deactivated_after_edit,			// The item was active and edited, but the interaction has ended.
	};


	// @brief Checks if the mouse is currently hovering over the current ImGui window.
	// @return True if the mouse is hovering over the window, false otherwise.
	bool is_holvering_window();

	// @brief Checks if the current ImGui item (e.g., button, text) is double-clicked.
	// @return True if the item is double-clicked, false otherwise.
	bool is_item_double_clicked();

	// @brief Determines the mouse interaction state (e.g., hovered, clicked) on the current ImGui item.
	// @return The mouse interaction state (e.g., hovered, clicked, held).
	mouse_interation get_mouse_interation_on_item(const bool block_input = false);

	// @brief Determines the mouse interaction state (e.g., hovered, clicked) on the current ImGui window.
	// @return The mouse interaction state (e.g., hovered, clicked, held).
	mouse_interation get_mouse_interation_on_window();

	// @brief Wraps text at underscores to fit within a specified width.
	// @param [text] The text to wrap.
	// @param [wrap_width] The maximum width before wrapping occurs.
	// @return The wrapped text as a string.
	std::string wrap_text_at_underscore(const std::string& text, float wrap_width);

	// @brief Sets the position of the next ImGui window based on a predefined location.
	// @param [location] The desired position of the window (e.g., center, top-left).
	// @param [padding] The padding to apply around the window.
	void set_next_window_pos(window_pos location, f32 padding = 10.f);

	// @brief Sets the position of the next ImGui window relative to the current window.
	// @param [location] The desired position of the window (e.g., center, top-left).
	// @param [padding] The padding to apply around the window.
	void set_next_window_pos_in_window(window_pos location, f32 padding = 10.f);

	// @brief Displays a menu to select the position of the next ImGui window.
	// @param [position] The current position of the window, which can be modified.
	// @param [show_window] A boolean flag to control the visibility of the window.
	void next_window_position_selector(window_pos& position, bool& show_window);

	// @brief Displays a popup menu to select the position of the next ImGui window.
	// @param [position] The current position of the window, which can be modified.
	// @param [show_window] A boolean flag to control the visibility of the window.
	void next_window_position_selector_popup(window_pos& position, bool& show_window);

	// @brief Draws a vertical separation line.
	void seperation_vertical();

	// @brief Creates a button with a gray color scheme.
	// @param [label] The label displayed on the button.
	// @param [size] The size of the button. If {0, 0}, the size is automatically calculated.
	// @return True if the button is clicked, false otherwise.
	bool gray_button(const char* label, const ImVec2& size = { 0, 0 });

	// @brief Creates a toggle button that changes its appearance based on a boolean variable.
	// @param [label] The label displayed on the button.
	// @param [bool_var] The boolean variable that controls the button's state.
	// @param [size] The size of the button. If {0, 0}, the size is automatically calculated.
	// @return True if the button is clicked, false otherwise.
	bool toggle_button(const char* lable, bool& bool_var, const ImVec2& size = { 0, 0 });

	// @brief Draws text using a larger font.
	// @param [text] The text to be drawn.
	void big_text(const char* text, bool wrapped = false);

	// @brief Displays text in a bold font.
	// @param [text] The text to display.
	// @param [wrapped] Whether the text should be wrapped if it exceeds the available width.
	void text_bold(const char* text, bool wrapped = false);

	// @brief Displays text in an italic font.
	// @param [text] The text to display.
	// @param [wrapped] Whether the text should be wrapped if it exceeds the available width.
	void text_italic(const char* text, bool wrapped = false);

	// @brief Displays text with a specific style (e.g., ancient text style).
	// @param [text] The text to display.
	void anci_text(std::string_view text);










	// @brief Displays a help marker with tooltip containing the provided description.
	// @param [desc] The description text to be displayed in the tooltip.
	void help_marker(const char* desc);

	// @brief Adjusts the current ImGui cursor position by adding the specified horizontal and vertical shift offsets.
	// @param [shift_x] The horizontal shift offset.
	// @param [shift_y] The vertical shift offset.
	void shift_cursor_pos(const f32 shift_x, const f32 shift_y);

	// @brief Adjusts the current ImGui cursor position by adding the specified horizontal and vertical shift offsets.
	// @param [shift_x] The horizontal shift offset.
	// @param [shift_y] The vertical shift offset.
	void shift_cursor_pos(const ImVec2 shift);

	void progressbar_with_text(const char* label, const char* progress_bar_text, f32 percent, f32 label_size = 50.f, f32 progressbar_size_x = 50.f, f32 progressbar_size_y = 1.f);

	// @brief This function sets up an ImGui table with two columns, where the first column is resizable and the second column fills the remaining availabel area
	// @brief CAUTION - you need to call UI::end_table() at the end of the table;
	// @param [label] Is used to identify the table
	bool begin_table(std::string_view label, bool display_name = true, ImVec2 size = ImVec2(0,0), f32 inner_width = 0.0f, bool set_columns_width = true, f32 columns_width_percentage = 0.5f);
	
	// @brief Ends the table started with UI::begin_table().
	void end_table();


	// @brief This function draws a custom frame with two separate sections: [left_side] and [right_side].
	//          The width of the first column is specified by [width_left_side]. Both sections are contained within
	//          the same ImGui table. Each section's content is drawn using the provided function callbacks (lamdas or functions)
	// @param [width_left_side] The fixed width of the first column.
	// @param [left_side] The function representing the content of the left side.
	// @param [right_side] The function representing the content of the right side.
	void custom_frame(const f32 width_left_side, std::function<void()> left_side, std::function<void()> right_side);

	// @brief Creates a custom frame with a left and right side, allowing for resizing and custom coloring.
	// @param [width_left_side] The width of the left side panel.
	// @param [can_resize] Whether the left side panel can be resized.
	// @param [color_left_side] The background color of the left side panel.
	// @param [left_side] A function to render the content of the left side panel.
	// @param [right_side] A function to render the content of the right side panel.
	void custom_frame_NEW(const f32 width_left_side, const bool can_resize, const ImU32 color_left_side, std::function<void()> left_side, std::function<void()> right_side);

	// @brief Creates a search input field with a clear button.
	// @param [lable] The label for the search input field.
	// @param [search_text] A reference to the string that holds the search text.
	// @return true if the search text was changed, false otherwise.
	bool serach_input(const char* lable, std::string& search_text);

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
	bool table_row_slider(std::string_view label, int& value, int min_value = 0, int max_value = 1, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

	// @brief Renders a table row with two columns, each containing custom content.
	// @param [first_colum] A function to render the content of the first column.
	// @param [second_colum] A function to render the content of the second column.
	void table_row(std::function<void()> first_colum, std::function<void()> second_colum);

	// @brief Renders a table row with a label and an editable text field.
	// @param [label] The label for the row.
	// @param [text] A reference to the string that holds the text.
	// @param [enable_input] A reference to a boolean that controls whether the text field is editable.
	void table_row(std::string_view label, std::string& text, bool& enable_input);

	// @brief Renders a table row with a label and formatted text.
	// @param [label] The label for the row.
	// @param [format] The format string for the text.
	// @param [...] Variable arguments for the format string.
	void table_row_text(std::string_view label, const char* format, ...);

	// @brief Renders a table row with a label and a checkbox.
	// @param [label] The label for the row.
	// @param [value] A reference to the boolean value controlled by the checkbox.
	void table_row(std::string_view label, bool& value);

	// @brief Renders a table row with a label and a non-editable text value.
	// @param [label] The label for the row.
	// @param [value] The text value to display.
	void table_row(std::string_view label, std::string_view value);

	// @brief Renders a table row with a label and a 4x4 matrix, allowing for editing of translation, rotation, and scale.
	// @param [label] The label for the row.
	// @param [value] A reference to the 4x4 matrix to be edited.
	// @return true if any component of the matrix was changed, false otherwise.
	bool table_row(std::string_view label, glm::mat4& value);

	// @brief Adds a row to an ImGui table with a label and corresponding value input field.
	// @tparam [T] The type of the value.
	// @param [label] The label for the row.
	// @param [value] The value to be displayed or edited.
	// @param [flags] Flags controlling the behavior of the input field.
	template<typename T>
	bool table_row(std::string_view label, T& value, f32 drag_speed = 0.01f, T min_value = T{0}, T max_value = T{1}, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None) {

		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 current_item_spacing = style.ItemSpacing;
		flags |= ImGuiInputTextFlags_AllowTabInput;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		size_t pos = label.find("##");
		std::string_view displayLabel = (pos != std::string_view::npos) ? label.substr(0, pos) : label;
		ImGui::Text("%.*s", static_cast<int>(displayLabel.length()), displayLabel.data());

		ImGui::TableSetColumnIndex(1);
		std::string loc_label = "##";
		loc_label += label.data();
		ImGui::SetNextItemWidth(ImGui::GetColumnWidth());

		if constexpr (std::is_same_v<T, bool>) {

			ImGui::Text("%s", util::bool_to_str(value));
			return false;
		}

		else if constexpr (std::is_integral_v<T>) {
			if constexpr (std::is_unsigned_v<T>) {
				switch (sizeof(T)) {
				case 1: return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_U8, &value, drag_speed, &min_value, &max_value, "%u", flags);		// u8
				case 2: return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_U16, &value, drag_speed, &min_value, &max_value, "%u", flags);	// u16
				case 4: return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_U32, &value, drag_speed, &min_value, &max_value, "%u", flags);	// u32
				case 8: return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_U64, &value, drag_speed, &min_value, &max_value, "%llu", flags);	// u64
				default:
					ImGui::Text("Could not display variable of type unsigned int [size: %llu]", sizeof(T));
					return false;
				}
			} else {
				switch (sizeof(T)) {
				case 1: return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_S8, &value, drag_speed, &min_value, &max_value, "%d", flags);		// i8
				case 2: return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_S16, &value, drag_speed, &min_value, &max_value, "%d", flags);	// i16
				case 4: return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_S32, &value, drag_speed, &min_value, &max_value, "%d", flags);	// i32
				case 8: return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_S64, &value, drag_speed, &min_value, &max_value, "%lld", flags);	// i64
				default:
					ImGui::Text("Could not display var of type signed int [size: %llu]", sizeof(T));
					return false;
				}
			}
		} else if constexpr (std::is_floating_point_v<T>) {
			if constexpr (sizeof(T) <= 4)
				return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_Float, &value, drag_speed, &min_value, &max_value, "%.3f", flags);
			else 
				return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_Double, &value, drag_speed, &min_value, &max_value, "%.3f", flags);
		}

		else if constexpr (std::is_same_v<T, glm::vec2> || std::is_same_v<T, ImVec2>)
			return ImGui::DragFloat2(loc_label.c_str(), &value[0], drag_speed, min_value[0], max_value[0], "%.2f", flags);

		else if constexpr (std::is_same_v<T, glm::vec3>)
			return ImGui::DragFloat3(loc_label.c_str(), &value[0], drag_speed, min_value[0], max_value[0], "%.2f", flags);

		else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>)
			return ImGui::DragFloat4(loc_label.c_str(), &value[0], drag_speed, min_value[0], max_value[0], "%.2f", flags);

		else if constexpr (std::is_convertible_v<T, std::string>) {

			ImGui::Text("%s", std::to_string(value).c_str());
			return false;
		}

		else
			ImGui::Text("Could not display variable");

		return false;
	}

	/*
		else if constexpr (std::is_same_v<T, int32> || std::is_same_v<T, u32> || std::is_same_v<T, int64> || std::is_same_v<T, u64>) {

			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			return ImGui::DragInt(loc_label.c_str(), &value, drag_speed, min_value, max_value, "%.2f", flags);
		}
	*/

	// @brief Renders a table row with a label and a progress bar.
	// @param [label] The label for the row.
	// @param [progress_bar_text] The text to display alongside the progress bar.
	// @param [percent] The percentage value of the progress bar.
	// @param [auto_resize] Whether the progress bar should automatically resize to fit the column width.
	// @param [progressbar_size_x] The width of the progress bar.
	// @param [progressbar_size_y] The height of the progress bar.
	void table_row_progressbar(std::string_view label, const char* progress_bar_text, const f32 percent, const bool auto_resize = true, const f32 progressbar_size_x = 50.f, const f32 progressbar_size_y = 1.f);

	// @brief Begins a collapsible header section with an indent.
	// @param [lable] The label for the collapsible header.
	// @return true if the header is open, false otherwise.
	bool begin_collapsing_header_section(const char* lable);

	// @brief Ends a collapsible header section and removes the indent.
	void end_collapsing_header_section();

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
	bool table_row_slider(std::string_view label, T& value, f32 min_value = 0.f, f32 max_value = 1.f, f32 draw_speed = 0.2f, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None) {

		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 current_item_spacing = style.ItemSpacing;
		flags |= ImGuiInputTextFlags_AllowTabInput;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);
		std::string loc_label = "##";
		loc_label += label.data();
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
	
	
	template<typename T>
	bool table_row_drag_scalar(std::string_view label, T& value, const char* format, T min_value = (T)0, T max_value = (T)1, f32 draw_speed = 0.2f, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None) {

		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 current_item_spacing = style.ItemSpacing;
		flags |= ImGuiInputTextFlags_AllowTabInput;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);
		std::string loc_label = "##";
		loc_label += label.data();
		ImGui::SetNextItemWidth(ImGui::GetColumnWidth());

		if constexpr (std::is_same_v<T, int>)
			return ImGui::SliderInt(loc_label.c_str(), &value, static_cast<int>(min_value), static_cast<int>(max_value), "%d", flags);

		if constexpr (std::is_same_v<T, f32> || std::is_same_v<T, f64>)
			return ImGui::SliderFloat(loc_label.c_str(), &value, min_value, max_value, "%.2f", flags);

		if constexpr (std::is_same_v<T, u32>)
			return ImGui::DragScalar(loc_label.c_str(), ImGuiDataType_U32, &value, 0.2f, (const void*)&min_value, (const void*)&max_value, format);

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