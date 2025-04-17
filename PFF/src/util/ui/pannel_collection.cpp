
#include "util/pffpch.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>



#include <emmintrin.h>    // SSE2
#include <immintrin.h>    // for future‑proofing (AVX)



#include "pannel_collection.h"

namespace PFF::UI {

	bool is_holvering_window() {

		const ImVec2 mouse_pos = ImGui::GetMousePos();
		const ImVec2 popup_pos = ImGui::GetWindowPos();
		const ImVec2 popup_size = ImGui::GetWindowSize();
		return (mouse_pos.x >= popup_pos.x && mouse_pos.x <= popup_pos.x + popup_size.x && mouse_pos.y >= popup_pos.y && mouse_pos.y <= popup_pos.y + popup_size.y);
	}
	
	bool is_item_double_clicked() {

		const ImVec2 item_pos = ImGui::GetItemRectMin();
		const ImVec2 item_max = item_pos + ImGui::GetItemRectSize();
		return ImGui::IsMouseHoveringRect(item_pos, item_max) && ImGui::IsMouseDoubleClicked(0);
	}

	void set_mouse_interaction_state(const ImGuiMouseButton_ mouse_button, mouse_interation& state, bool& is_button_down) {

		u8 offset = 0;
		if (mouse_button == ImGuiMouseButton_Right)
			offset = 4;
		if (mouse_button == ImGuiMouseButton_Middle)
			offset = 8;

		// Check for left mouse button interactions
		if (ImGui::IsMouseDoubleClicked(mouse_button))
			state = static_cast<mouse_interation>(static_cast<u8>(mouse_interation::left_double_clicked) + offset);
			
		else if (ImGui::IsMouseClicked(mouse_button))
			state = static_cast<mouse_interation>(static_cast<u8>(mouse_interation::left_clicked) + offset);

		else if (ImGui::IsMouseDragging(mouse_button))
			state = mouse_interation::dragged;

		else if (ImGui::IsMouseDown(mouse_button)) {
			is_button_down = true;
			state = static_cast<mouse_interation>(static_cast<u8>(mouse_interation::left_pressed) + offset);
		
		} else if (ImGui::IsMouseReleased(mouse_button) && is_button_down) {
			is_button_down = false;
			state = static_cast<mouse_interation>(static_cast<u8>(mouse_interation::left_released) + offset);
		}
	}

	mouse_interation get_mouse_interation_on_item(const bool block_input) {

		if (block_input)
			return mouse_interation::none;
		
		static bool is_middle_button_down = false;
		static bool is_right_button_down = false;
		static bool is_left_button_down = false;

		const ImVec2 item_pos = ImGui::GetItemRectMin();
		const ImVec2 item_max = item_pos + ImGui::GetItemRectSize();
		if (!ImGui::IsMouseHoveringRect(item_pos, item_max))							// If the mouse is not hovering over the item, return none
			return mouse_interation::none;

		mouse_interation state = mouse_interation::hovered;								// default to hover state

		if (ImGui::IsItemFocused())
			state = mouse_interation::focused;

		if (ImGui::IsItemActive())
			state = mouse_interation::active;

		if (ImGui::IsItemDeactivated())
			state = mouse_interation::deactivated;

		if (ImGui::IsItemDeactivatedAfterEdit())
			state = mouse_interation::deactivated_after_edit;
			
		set_mouse_interaction_state(ImGuiMouseButton_Middle, state, is_middle_button_down);			// least importent one first
		set_mouse_interaction_state(ImGuiMouseButton_Right, state, is_right_button_down);
		set_mouse_interaction_state(ImGuiMouseButton_Left, state, is_left_button_down);

		return state;
	}

	mouse_interation get_mouse_interation_on_window() {

		static bool is_middle_button_down = false;
		static bool is_right_button_down = false;
		static bool is_left_button_down = false;

		// If the mouse is not hovering over the item, return none
		const ImVec2 item_pos = ImGui::GetWindowPos();
		const ImVec2 item_max = item_pos + ImGui::GetWindowSize();
		if (!ImGui::IsMouseHoveringRect(item_pos, item_max))
			return mouse_interation::none;

		mouse_interation state = mouse_interation::hovered;								// default to hover state

		if (ImGui::IsItemFocused())
			state = mouse_interation::focused;

		if (ImGui::IsItemActive())
			state = mouse_interation::active;

		if (ImGui::IsItemDeactivated())
			state = mouse_interation::deactivated;

		if (ImGui::IsItemDeactivatedAfterEdit())
			state = mouse_interation::deactivated_after_edit;
			
		set_mouse_interaction_state(ImGuiMouseButton_Middle, state, is_middle_button_down);			// least importent one first
		set_mouse_interaction_state(ImGuiMouseButton_Right, state, is_right_button_down);
		set_mouse_interaction_state(ImGuiMouseButton_Left, state, is_left_button_down);

		return state;
	}

	
	FORCEINLINE bool is_delim_char(char c) { return c==' '||c=='_'||c=='-'||c=='/'||c=='\\'||c=='.'; }


	void wrap_text(std::string& string, f32 wrap_width, int max_lines) {

		size_t orig = string.size();
		string.reserve(orig * 2 + 16);
		size_t read_pos   = 0, write_pos = 0;
		size_t line_start = 0;
		size_t last_delim = std::string::npos;
		int    line_count = 0;
	
		// SIMD constants
		__m128i space  = _mm_set1_epi8(' ');
		__m128i uscore = _mm_set1_epi8('_');
		__m128i dash   = _mm_set1_epi8('-');
		__m128i slash  = _mm_set1_epi8('/');
		__m128i bslash = _mm_set1_epi8('\\');
		__m128i dot    = _mm_set1_epi8('.');
	
		auto insert_char = [&](size_t pos, char c) {
			string.insert(string.begin() + pos, c);
			write_pos++;
		};
	
		while (read_pos < string.size()) {
			size_t rem   = string.size() - read_pos;
			size_t chunk = rem >= 16 ? 16 : rem;
			__m128i block = _mm_loadu_si128((__m128i*)(string.data()+read_pos));
			__m128i m = _mm_or_si128(
				_mm_or_si128(_mm_cmpeq_epi8(block, space),  _mm_cmpeq_epi8(block, uscore)),
				_mm_or_si128(_mm_or_si128(_mm_cmpeq_epi8(block, dash),
										  _mm_cmpeq_epi8(block, slash)),
							 _mm_or_si128(_mm_cmpeq_epi8(block, bslash),
										  _mm_cmpeq_epi8(block, dot)))
			);
	
			for (size_t i = 0; i < chunk; i++) {

				char c = string[read_pos + i];
				if (write_pos != read_pos + i) string[write_pos] = c;
					write_pos++;
				if (is_delim_char(c))
					last_delim = write_pos;
	
				// Measure only when potential overflow:
				ImVec2 string_size = ImGui::CalcTextSize(string.c_str() + line_start, string.c_str() + write_pos);
				if (string_size.x > wrap_width) {
					bool done = false;
	
					// If next line would exceed max_lines, ellipsize THIS line instead of breaking:
					if (max_lines > 0 && line_count + 1 == max_lines) {
						size_t end = write_pos;															// find end-of-line = write_pos
						while (end > line_start && ImGui::CalcTextSize(string.c_str()+line_start, string.c_str()+end).x + ImGui::CalcTextSize("...").x > wrap_width)			// back up so we can fit "..."
							--end;

						string.replace(end, std::string::npos, "...");									// replace tail with "..."
						string.resize(end + 3);															// truncate everything after
						return;
					}
	
					if (last_delim != std::string::npos && last_delim > line_start) {					// Otherwise do normal break
						insert_char(last_delim, '\n');
						line_start = last_delim + 1;
					} else {																			// forced break: replace last char with '-' then newline
						size_t brk = write_pos - 1;
						string[brk] = '-';
						insert_char(brk+1, '\n');
						line_start = brk + 2;
					}
					line_count++;
					last_delim = std::string::npos;
					done = false; 																		// continue processing rest
					break;
				}
			}
			read_pos += chunk;
		}
		string.resize(write_pos);
	}

	
	std::string wrap_text_at_underscore(const std::string& text, float wrap_width) {

		std::stringstream ss(text);
		std::string segment;
		std::string wrapped_text;
		float text_width = 0.0f;

		// Split the text at underscores
		while (std::getline(ss, segment, '_')) {
			// Check if adding this segment exceeds the wrap width
			if (text_width + ImGui::CalcTextSize(segment.c_str()).x > wrap_width) {
				wrapped_text += "\n"; // Insert a line break if needed
				text_width = 0.0f; // Reset the line width counter
			}
			wrapped_text += segment + "_"; // Add the segment with underscore
			text_width += ImGui::CalcTextSize((wrapped_text + "_").c_str()).x; // Update the line width
		}

		// Remove the trailing underscore
		if (!wrapped_text.empty() && wrapped_text.back() == '_') {
			wrapped_text.pop_back();
		}

		return wrapped_text;
	}


	void set_next_window_pos(window_pos location, f32 padding) {

		if (location == window_pos::center)
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		else if (static_cast<u32>(location) >= 2) {

			const f32 title_bar_height = 60.f;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_pos = { viewport->WorkPos.x , viewport->WorkPos.y + title_bar_height }; // Use work area to avoid menu-bar/task-bar, if any!
			ImVec2 work_size = viewport->WorkSize;
			ImVec2 window_pos, window_pos_pivot;

			window_pos.x = (location == window_pos::top_right || location == window_pos::bottom_right) ? (work_pos.x + work_size.x - padding) : (work_pos.x + padding);
			window_pos.y = (location == window_pos::bottom_right || location == window_pos::bottom_left) ? (work_pos.y + work_size.y - padding - title_bar_height) : (work_pos.y + padding);
			window_pos_pivot.x = (location == window_pos::top_right || location == window_pos::bottom_right) ? 1.0f : 0.0f;
			window_pos_pivot.y = (location == window_pos::bottom_right || location == window_pos::bottom_left) ? 1.0f : 0.0f;
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowViewport(viewport->ID);
		}
	}


	void set_next_window_pos_in_window(window_pos location, f32 padding) {

		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();

		if (location == window_pos::center)
			ImGui::SetNextWindowPos(ImVec2(pos.x + (size.x / 2), pos.y + (size.y / 2)), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		else if (static_cast<u32>(location) >= 2) {

			bool has_tab_bar = !(GImGui->CurrentWindow->Flags & ImGuiWindowFlags_NoTitleBar);

			const ImGuiViewport* viewport = ImGui::GetWindowViewport();
			ImVec2 window_pos, window_pos_pivot;

			window_pos.x = (location == window_pos::top_right || location == window_pos::bottom_right) ? (pos.x + size.x - padding) : (pos.x + padding);
			window_pos.y = (location == window_pos::bottom_right || location == window_pos::bottom_left) ? (pos.y + size.y - padding) : (pos.y + padding + (has_tab_bar ? ImGui::GetFrameHeight() : 0.f));
			window_pos_pivot.x = (location == window_pos::top_right || location == window_pos::bottom_right) ? 1.0f : 0.0f;
			window_pos_pivot.y = (location == window_pos::bottom_right || location == window_pos::bottom_left) ? 1.0f : 0.0f;
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowViewport(viewport->ID);
		}
	}


	void next_window_position_selector(window_pos& position, bool& show_window) {

		if (ImGui::MenuItem("custom", NULL, position == window_pos::custom))
			position = window_pos::custom;

		if (ImGui::MenuItem("center", NULL, position == window_pos::center))
			position = window_pos::center;

		if (ImGui::MenuItem("top-left", NULL, position == window_pos::top_left))
			position = window_pos::top_left;

		if (ImGui::MenuItem("top-right", NULL, position == window_pos::top_right))
			position = window_pos::top_right;

		if (ImGui::MenuItem("bottom-left", NULL, position == window_pos::bottom_left))
			position = window_pos::bottom_left;

		if (ImGui::MenuItem("bottom-right", NULL, position == window_pos::bottom_right))
			position = window_pos::bottom_right;

		if (show_window && ImGui::MenuItem("close"))
			show_window = false;
	}
	
	void next_window_position_selector_popup(window_pos& position, bool& show_window) {

		if (ImGui::BeginPopupContextWindow()) {

			next_window_position_selector(position, show_window);

			ImGui::EndPopup();
		}
	}

	
	void adjust_popup_to_window_bounds(const ImVec2 expected_popup_size) {

		ImVec2 popup_pos = ImGui::GetMousePos();
		// ImVec2 mouse_pos = ImGui::GetMousePos();
		// ImVec2 expected_size = ImVec2(20, 50);							// TODO: Adjust based on content

		// if (file_currupted) 
		// 	switch (loc_file_curruption_source) {					// sprocimate size bycorruption type
		// 		default:
		// 		case file_curruption_source::unknown:				expected_size = ImVec2(280, 250); break;	// should display everything to help user
		// 		case file_curruption_source::header_incorrect:		expected_size = ImVec2(280, 250); break;	// should display header
		// 		case file_curruption_source::empty_file:			expected_size = ImVec2(180, 150); break;	// dosnt need to display anything other than size
		// 	}
		
		// popup_pos = mouse_pos;
		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 window_size = ImGui::GetWindowSize();
		
		if ((popup_pos.x + expected_popup_size.x) > (window_pos.x + window_size.x))				// Check if popup would go out of bounds horizontally
			popup_pos.x = window_pos.x + window_size.x - expected_popup_size.x;
		
		if ((popup_pos.y + expected_popup_size.y) > (window_pos.y + window_size.y))				// Check vertical bounds
			popup_pos.y = window_pos.y + window_size.y - expected_popup_size.y;
		
		ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Appearing);
	}


	void seperation_vertical() {

		ImGui::SameLine();
		shift_cursor_pos(5, 0);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		shift_cursor_pos(5, 0);
	}


	bool gray_button(const char* lable, const ImVec2& size) {

		ImGui::PushStyleColor(ImGuiCol_Button, UI::get_default_gray_ref());
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::get_action_color_gray_hover_ref());
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::get_action_color_gray_active_ref());

		const bool result = ImGui::Button(lable, size);

		ImGui::PopStyleColor(3);
		return result;
	}

	bool toggle_button(const char* lable, bool& bool_var, const ImVec2& size) {

		// show weaker color if toggle_bool is false
		if (!bool_var) {

			ImGui::PushStyleColor(ImGuiCol_Button, UI::get_action_color_00_faded_ref());
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::get_action_color_00_weak_ref());
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::get_action_color_00_default_ref());
		}

		const bool result = ImGui::Button("show Markdown", ImVec2(100, 21));

		if (!bool_var)
			ImGui::PopStyleColor(3);

		return result;
	}

	void big_text(const char* text, bool wrapped) {

		ImGui::PushFont(application::get().get_imgui_layer()->get_font("regular_big"));

		if (wrapped)
			ImGui::TextWrapped("%s", text);
		else
			ImGui::Text("%s", text);

		ImGui::PopFont();
	}


	void text_bold(const char* text, bool wrapped) {

		ImGui::PushFont(application::get().get_imgui_layer()->get_font("bold"));

		if (wrapped)
			ImGui::TextWrapped("%s", text);
		else
			ImGui::Text("%s", text);

		ImGui::PopFont();
	}


	void text_italic(const char* text, bool wrapped) {

		ImGui::PushFont(application::get().get_imgui_layer()->get_font("italic"));

		if (wrapped)
			ImGui::TextWrapped("%s", text);
		else
			ImGui::Text("%s", text);

		ImGui::PopFont();
	}

	// ================================================================================================================================================================================================
	// ANCI escape code parsing
	// ================================================================================================================================================================================================

	inline ImVec4 get_background_8_color(int index) {
		const static ImVec4 colors[] = {
			ImVec4(0.0f, 0.0f, 0.0f, 1.0f),         // 40: Black
			ImVec4(0.5f, 0.0f, 0.0f, 1.0f),         // 41: Red
			ImVec4(0.0f, 0.5f, 0.0f, 1.0f),         // 42: Green
			ImVec4(0.5f, 0.5f, 0.0f, 1.0f),         // 43: Yellow
			ImVec4(0.0f, 0.0f, 0.5f, 1.0f),         // 44: Blue
			ImVec4(0.5f, 0.0f, 0.5f, 1.0f),         // 45: Magenta
			ImVec4(0.0f, 0.5f, 0.5f, 1.0f),         // 46: Cyan
			ImVec4(0.75f, 0.75f, 0.75f, 1.0f),      // 47: White
		};
		if (index >= 0 && index < 8)
			return colors[index];
		return ImVec4(0.0f, 0.0f, 0.0f, 0.0f);      // Transparent default
	}

	inline ImVec4 get_background_16_color(int index) {
		const static ImVec4 colors[] = {
			ImVec4(0.5f, 0.5f, 0.5f, 1.0f),        // 100: Bright Black (Gray)
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f),        // 101: Bright Red
			ImVec4(0.0f, 1.0f, 0.0f, 1.0f),        // 102: Bright Green
			ImVec4(1.0f, 1.0f, 0.0f, 1.0f),        // 103: Bright Yellow
			ImVec4(0.0f, 0.0f, 1.0f, 1.0f),        // 104: Bright Blue
			ImVec4(1.0f, 0.0f, 1.0f, 1.0f),        // 105: Bright Magenta
			ImVec4(0.0f, 1.0f, 1.0f, 1.0f),        // 106: Bright Cyan
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f),        // 107: Bright White
		};
		if (index >= 0 && index < 8)
			return colors[index];
		return ImVec4(0.0f, 0.0f, 0.0f, 0.0f);      // Transparent default
	}

	inline ImVec4 get_8_color(int index) {
		const static ImVec4 colors[] = {
			ImVec4(0.0f, 0.0f, 0.0f, 1.0f),         // 0: Black
			ImVec4(0.9f, 0.1f, 0.1f, 1.0f),         // 1: Red
			ImVec4(0.0f, 0.5f, 0.0f, 1.0f),         // 2: Green
			ImVec4(0.9f, 0.9f, 0.1f, 1.0f),         // 3: Yellow
			ImVec4(0.0f, 0.0f, 0.5f, 1.0f),         // 4: Blue
			ImVec4(0.5f, 0.0f, 0.5f, 1.0f),         // 5: Magenta
			ImVec4(0.0f, 0.5f, 0.5f, 1.0f),         // 6: Cyan
			ImVec4(0.75f, 0.75f, 0.75f, 1.0f),      // 7: White
		};
		if (index >= 0 && index < 8)
			return colors[index];
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	inline ImVec4 get_16_color(int index) {
		const static ImVec4 colors[] = {
			ImVec4(0.5f, 0.5f, 0.5f, 1.0f),         // 8: Bright Black (Gray)
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f),         // 9: Bright Red
			ImVec4(0.0f, 1.0f, 0.0f, 1.0f),         // 10: Bright Green
			ImVec4(1.0f, 1.0f, 0.0f, 1.0f),         // 11: Bright Yellow
			ImVec4(0.0f, 0.0f, 1.0f, 1.0f),         // 12: Bright Blue
			ImVec4(1.0f, 0.0f, 1.0f, 1.0f),         // 13: Bright Magenta
			ImVec4(0.0f, 1.0f, 1.0f, 1.0f),         // 14: Bright Cyan
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f),         // 15: Bright White
		};
		if (index >= 8 && index < 16)
			return colors[index - 8];
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	inline ImVec4 get_256_color(int index) {

		index = math::clamp(index, 0, 255);
		if (index < 16)
			return index < 8 ? get_8_color(index) : get_16_color(index);

		else if (index < 232) {

			const int cube_index = index - 16;
			const int r = cube_index / 36;
			const int g = (cube_index % 36) / 6;
			const int b = cube_index % 6;
			const int values[] = { 0, 95, 135, 175, 215, 255 };
			return ImVec4(values[r] / 255.0f, values[g] / 255.0f, values[b] / 255.0f, 1.0f);

		}
		else {

			const int gray = 8 + (index - 232) * 10;
			const float intensity = gray / 255.0f;
			return ImVec4(intensity, intensity, intensity, 1.0f);
		}
	}

	void render_text_segment(const char* text, int length, ImVec4 fg_color, ImVec4 bg_color) {

		ImVec2 pos = ImGui::GetCursorScreenPos();
		const ImVec2 text_size = ImGui::CalcTextSize(text, text + length);

		if (bg_color.w > 0.0f)                                                                                                                  // Draw background if needed
			ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + text_size.x, pos.y + text_size.y), ImGui::GetColorU32(bg_color));

		ImGui::TextColored(fg_color, "%.*s", length, text);
		ImGui::SameLine(0, 0);
	}

	void anci_text(std::string_view text) {

		size_t current_position = 0;
		ImVec4 color = ImVec4(1.f);
		ImVec4 bg_color = ImVec4(0.f);

		while (current_position < text.length()) {

			size_t anci_start = text.find('\033', current_position);

			if (anci_start >= text.length() || anci_start == std::string::npos) {																// No more ANSI codes, print the remaining text
				render_text_segment(text.data() + current_position, static_cast<int>(text.length() - current_position), color, bg_color);
				break;
			}

			if (anci_start > current_position)
				render_text_segment(text.data() + current_position, static_cast<int>(anci_start - current_position), color, bg_color);

			current_position = anci_start;
			size_t anci_end = text.find('m', anci_start);
			VALIDATE(anci_end != std::string::npos, break, "", "Detected ANCS escape code is not closed [" << text << "] at pos [" << anci_start << "]");

			if (anci_start + 1 >= text.length() || text[anci_start + 1] != '[')                                                                 // Check if ANSI code starts with '['
				current_position = anci_start + 1;

			// Parse ANSI code
			const std::string_view code = text.substr(anci_start, anci_end - anci_start);
			if (code.size() > 2 && code[0] == '\033' && code[1] == '[') {
				std::vector<int> params;
				size_t pos = 2;

				while (pos < code.size()) {
					const size_t end = code.find(';', pos);
					const auto sub = code.substr(pos, (end == std::string::npos ? code.size() : end) - pos);

					int value = 0;
					auto result = std::from_chars(sub.data(), sub.data() + sub.size(), value);
					if (result.ec == std::errc())
						params.push_back(value);

					pos = end != std::string::npos ? end + 1 : code.size();
				}

				// Process parameters
				for (size_t i = 0; i < params.size();) {
					const int p = params[i++];

					if (p == 0) {                                               // Reset
						color = ImVec4(1.f);
						bg_color = ImVec4(0.f);
					}
					else if (p == 1) {                                          // Bold
						color.x = std::min(color.x * 1.2f, 1.0f);
						color.y = std::min(color.y * 1.2f, 1.0f);
						color.z = std::min(color.z * 1.2f, 1.0f);
					}

					// Foreground colors
					else if (p >= 30 && p <= 37)
						color = get_8_color(p - 30);

					else if (p >= 90 && p <= 97)
						color = get_16_color(p - 90 + 8);

					else if (p == 38) {                                         // Extended foreground
						if (i < params.size()) {
							const int type = params[i++];
							if (type == 5 && i < params.size())
								color = get_256_color(params[i++]);

							else if (type == 2 && i + 2 < params.size())
								color = ImVec4(params[i++] / 255.0f, params[i++] / 255.0f, params[i++] / 255.0f, 1.0f);
						}
					}

					// Background colors
					else if (p >= 40 && p <= 47)
						bg_color = get_background_8_color(p - 40);

					else if (p >= 100 && p <= 107)
						bg_color = get_background_16_color(p - 100);

					else if (p == 48) {                                         // Extended background
						if (i < params.size()) {
							const int type = params[i++];
							if (type == 5 && i < params.size())
								bg_color = get_256_color(params[i++]);

							else if (type == 2 && i + 2 < params.size())
								bg_color = ImVec4(params[i++] / 255.0f, params[i++] / 255.0f, params[i++] / 255.0f, 1.0f);
						}
					}
					else if (p == 49)                                           // Reset background
						bg_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
				}
			}

			current_position = anci_end + 1;
		}

		ImGui::NewLine();
	}

	// ================================================================================================================================================================================================
	// ANCI escape code parsing
	// ================================================================================================================================================================================================

	void shift_cursor_pos(const f32 shift_x, const f32 shift_y) {

		auto current_pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos({ current_pos.x + shift_x, current_pos.y + shift_y });
	}

	void shift_cursor_pos(const ImVec2 shift) {

		auto current_pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos({ current_pos.x + shift.x, current_pos.y + shift.y });
	}

	void progressbar_with_text(const char* lable, const char* progress_bar_text, f32 percent, f32 lable_size, f32 progressbar_size_x, f32 progressbar_size_y) {

		//PFF_PROFILE_FUNCTION();

		ImVec2 curser_pos;
		ImVec2 progressbar_size;
		ImVec2 text_size;
		f32 loc_lable_size;

		ImGuiStyle* style = &ImGui::GetStyle();
		curser_pos = ImGui::GetCursorPos();

		loc_lable_size = ImGui::CalcTextSize(lable).x;
		loc_lable_size = std::max<f32>(loc_lable_size, lable_size);
		ImGui::Text("%s", lable);
		ImGui::SetCursorPos({ curser_pos.x + loc_lable_size, curser_pos.y });

		text_size = ImGui::CalcTextSize(progress_bar_text);
		progressbar_size = text_size;
		progressbar_size.x = std::max<f32>(progressbar_size.x, progressbar_size_x);
		progressbar_size.y = std::max<f32>(progressbar_size.y, progressbar_size_y);

		curser_pos = ImGui::GetCursorPos();
		ImGui::ProgressBar(percent, progressbar_size, "");
		ImGui::SetCursorPos({ curser_pos.x + progressbar_size.x - (text_size.x + style->ItemSpacing.x), curser_pos.y });
		ImGui::Text("%s", progress_bar_text);
	}

	void help_marker(const char* desc) {

		ImGui::TextDisabled(" ? ");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}



	bool begin_table(std::string_view lable, bool display_name, ImVec2 size, f32 inner_width, bool set_columns_width, f32 columns_width_percentage) {

		if (display_name)
			ImGui::Text("%s:", lable.data());

		ImGuiTableFlags flags = ImGuiTableFlags_Resizable;
		if (ImGui::BeginTable(lable.data(), 2, flags, size, inner_width)) {

			// setup table and columns
			if (size.x > 0.0f && set_columns_width) {

				float column_width = size.x * columns_width_percentage;
				ImGui::TableSetupColumn("##one", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed, column_width);
				ImGui::TableSetupColumn("##two", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthStretch);
			} else {

				ImGui::TableSetupColumn("##one", ImGuiTableColumnFlags_NoHeaderLabel);
				ImGui::TableSetupColumn("##two", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_NoResize);
			}

			return true;
		}
		return false;
	}

#define GRAY(value)		IM_COL32(value, value, value, 255);

	void end_table() { ImGui::EndTable(); }

	void custom_frame(const f32 width_left_side, std::function<void()> left_side, std::function<void()> right_side) {

		ImGuiTableFlags flags = ImGuiTableFlags_None;
		//ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
		if (ImGui::BeginTable("test_master_divider", 2, flags)) {

			//ImGui::PopStyleVar();

			// setup column
			ImGui::TableSetupColumn("##one", ImGuiTableColumnFlags_WidthFixed, width_left_side);
			ImGui::TableSetupColumn("##two", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, ImGui::GetWindowWidth() - (width_left_side + 10.f));

			// enter first column
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			// draw background
			auto main_color = GRAY(40);
			const ImVec2 uperleft_corner = ImGui::GetCursorScreenPos();
			const ImVec2 lowerright_corner = { uperleft_corner.x + width_left_side ,uperleft_corner.y + ImGui::GetWindowHeight() };
			ImGui::GetWindowDrawList()->AddRectFilled(uperleft_corner, lowerright_corner, main_color /*PFF::UI::convert_color_to_int(color)*/);

			shift_cursor_pos(0, ImGui::GetTextLineHeight());
			ImGui::BeginGroup();
			left_side();
			ImGui::EndGroup();

			ImGui::TableSetColumnIndex(1);
			shift_cursor_pos(10, 10);

			//ImGuiStyleVar_WindowPadding,       // ImVec2    WindowPadding
			//ImGuiStyleVar_FramePadding,        // ImVec2    FramePadding
			//ImGuiStyleVar_CellPadding,         // ImVec2    CellPadding

			ImGui::BeginGroup();
			right_side();
			ImGui::EndGroup();

			ImGui::EndTable();
		}
	}
	
	void custom_frame_NEW(const f32 width_left_side, const bool can_resize, const ImU32 color_left_side, std::function<void()> left_side, std::function<void()> right_side) {

		static ImGuiChildFlags child_flags = ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding;
		auto content_region = ImGui::GetContentRegionAvail();
		const ImVec2 start_pos = ImGui::GetCursorScreenPos();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, color_left_side);
		ImGui::BeginChild("LEFT_SIDE", ImVec2(width_left_side, content_region.y), can_resize ? child_flags | ImGuiChildFlags_ResizeX : child_flags, ImGuiWindowFlags_None);
		ImGui::PopStyleColor();

		UI::shift_cursor_pos(2, 4);
		left_side();
		
		{
			const auto width = ImGui::GetContentRegionAvail().x;
			
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImU32 begin_col = IM_COL32(0, 0, 0, 0);
			ImU32 end_col = IM_COL32(0, 0, 0, 110);
			const f32 padding = ImGui::GetStyle().WindowPadding.x;

			draw_list->AddRectFilledMultiColor(
				ImVec2(start_pos.x + width + padding - 40, start_pos.y-20),
				ImVec2(start_pos.x + width + padding, start_pos.y + content_region.y),
				begin_col, end_col, end_col, begin_col);
			
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::SameLine();
		content_region = ImGui::GetContentRegionAvail();
		ImGui::BeginChild("RIGHT_SIDE", content_region, child_flags, ImGuiWindowFlags_None);

		right_side();

		ImGui::EndChild();
	}

	bool serach_input(const char* lable, std::string& search_text) {

		std::string buffer = search_text;
		buffer.resize(256);

		ImGui::SetNextItemAllowOverlap();
		if (ImGui::InputTextWithHint(lable, "Search", buffer.data(), 255, ImGuiInputTextFlags_EnterReturnsTrue)) {

			buffer.resize(strlen(buffer.c_str()));
			search_text = buffer;
			return true;
		}

		if (!search_text.empty()) {

			ImGui::SameLine();
			UI::shift_cursor_pos(-ImGui::CalcTextSize(" X ").x - (ImGui::GetStyle().ItemSpacing.x * 3), 0);

			std::string button_text = " X ##Clear_search_query__" + std::string(lable);
			if (ImGui::Button(button_text.c_str())) {

				LOG(Trace, "Trigger clear button")
				search_text = "";
			}
		}

		return false;
	}

	bool table_row_slider(std::string_view label, int& value, int min_value, int max_value, ImGuiInputTextFlags flags) {

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
		return ImGui::SliderInt(loc_label.c_str(), &value, static_cast<int>(min_value), static_cast<int>(max_value), "%d", flags);
	}

	void table_row(std::function<void()> first_colum, std::function<void()> second_colum) {

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		first_colum();
		ImGui::TableSetColumnIndex(1);
		second_colum();
	}

	void table_row(std::string_view label, std::string& text, bool& enable_input) {

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);

		if (enable_input) {

			std::string loc_label = "##";
			loc_label.reserve(label.size() + 2);
			std::remove_copy_if(label.begin(), label.end(), std::back_inserter(loc_label), [](char c) { return std::isspace(static_cast<unsigned char>(c)); });

			std::string buffer = text;
			buffer.resize(256);

			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			if (ImGui::InputText(loc_label.c_str(), buffer.data(), 256, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue)) {

				buffer.resize(strlen(buffer.c_str()));
				if (!buffer.empty()) {

					text = buffer;
					enable_input = false;
				}
			}

		} else {

			UI::gray_button(text.c_str());
			if (get_mouse_interation_on_item() == mouse_interation::left_double_clicked) 
				enable_input = true;
		}

	}

	
	void table_row_text(std::string_view label, const char* format, ...) {

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);

		va_list args;
		va_start(args, format);
		ImGui::TextV(format, args);
		va_end(args);
	}

	void table_row(std::string_view label, bool& value) {

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);

		ImGui::Checkbox(label.data(), &value);
		//ImGui::Text("%s", value.data());
	}

	void table_row(std::string_view label, std::string_view value) {

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);

		ImGui::Text("%s", value.data());
	}

	bool table_row(std::string_view label, glm::mat4& value, const bool display_in_degree) {
		
		glm::vec3 translation, rotation, scale;
		math::decompose_transform(value, translation, rotation, scale);

		if (display_in_degree)
			rotation = glm::degrees(rotation);

		const bool changed_0 = UI::table_row("translation", translation);
		const bool changed_1 = UI::table_row("rotation", rotation, display_in_degree ? 0.1f : 0.01f, glm::vec3(0), glm::vec3(0));
		const bool changed_2 = UI::table_row("scale", scale);

		if (changed_0 || changed_1 || changed_2) {

			if (display_in_degree)
				rotation = glm::radians(rotation);

			math::compose_transform(value, translation, rotation, scale);
		}

		return changed_0 || changed_1 || changed_2;
	}

	void table_row_progressbar(std::string_view label, const char* progress_bar_text, const f32 percent, const bool auto_resize, const f32 progressbar_size_x, const f32 progressbar_size_y) {


		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);

		float column_width = progressbar_size_x;
		if (auto_resize)
			if (ImGuiTable* table = ImGui::GetCurrentTable())
				column_width = table->Columns[1].WidthGiven;
		
		UI::progressbar_with_text("", progress_bar_text, percent, 0.0f, column_width, progressbar_size_y);
	}

	bool begin_collapsing_header_section(const char* lable) {

		ImGui::Indent();
		bool buffer = ImGui::CollapsingHeader(lable, ImGuiTreeNodeFlags_DefaultOpen);

		if (!buffer)
			ImGui::Unindent();

		return buffer;
	}

	void end_collapsing_header_section() {
		ImGui::Unindent();
	}

}
