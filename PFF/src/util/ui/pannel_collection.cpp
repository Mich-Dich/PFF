
#include "util/pffpch.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include "pannel_collection.h"

namespace PFF::UI {


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


	void seperation_vertical() {

		ImGui::SameLine();
		shift_cursor_pos(5, 0);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		shift_cursor_pos(5, 0);
	}


	bool gray_button(const char* lable, const ImVec2& size) {

		ImGui::PushStyleColor(ImGuiCol_Button, UI::action_color_gray_default);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::action_color_gray_hover);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::action_color_gray_active);

		const bool result = ImGui::Button(lable, size);

		ImGui::PopStyleColor(3);
		return result;
	}
	
	bool toggle_button(const char* lable, bool& bool_var, const ImVec2& size) {

		// show weaker color if toggle_bool is false
		if (!bool_var) {

			ImGui::PushStyleColor(ImGuiCol_Button, UI::action_color_00_faded);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::action_color_00_weak);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::action_color_00_default);
		}

		const bool result = ImGui::Button("show Markdown", ImVec2(100, 21));

		if (!bool_var)
			ImGui::PopStyleColor(3);

		return result;
	}

	void big_text(const char* text, bool wrapped) {

		ImGui::PushFont(application::get().get_imgui_layer()->get_font("regular_big"));

		if (wrapped)
			ImGui::TextWrapped(text);
		else
			ImGui::Text(text);

		ImGui::PopFont();
	}


	void text_bold(const char* text, bool wrapped) {

		ImGui::PushFont(application::get().get_imgui_layer()->get_font("bold"));

		if (wrapped)
			ImGui::TextWrapped(text);
		else
			ImGui::Text(text);

		ImGui::PopFont();
	}


	void text_italic(const char* text, bool wrapped) {

		ImGui::PushFont(application::get().get_imgui_layer()->get_font("italic"));

		if (wrapped)
			ImGui::TextWrapped(text);
		else
			ImGui::Text(text);

		ImGui::PopFont();
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

		ImGui::PushStyleColor(ImGuiCol_ChildBg, color_left_side);
		ImGui::BeginChild("Red", ImVec2(width_left_side, content_region.y), can_resize ? child_flags | ImGuiChildFlags_ResizeX : child_flags, ImGuiWindowFlags_None);
		ImGui::PopStyleColor();

		left_side();

		ImGui::EndChild();
		ImGui::SameLine();
		content_region = ImGui::GetContentRegionAvail();
		ImGui::BeginChild("Green", content_region, child_flags, ImGuiWindowFlags_None);

		right_side();

		ImGui::EndChild();

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

	void table_row(std::string_view label, std::string_view value) {

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%s", label.data());

		ImGui::TableSetColumnIndex(1);

		ImGui::Text("%s", value.data());
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

}
