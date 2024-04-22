
#include "util/pffpch.h"

#include <imgui.h>

#define UI_THEME_IMPLEMENTATION

#include "imgui_util.h"

namespace PFF::UI::imgui::util {


	//ImFont* get_font(const std::string& name) {
	//
	//	if (!contains(s_fonts, name))
	//		return nullptr;
	//
	//	return s_fonts.at(name);
	//}


	void set_next_window_pos(window_pos location, f32 padding) {

		PFF_PROFILE_FUNCTION();

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


	void next_window_position_selector(window_pos& position, bool& show_window) {

		if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::MenuItem("custom", NULL, position == window_pos::custom))
				position = window_pos::custom;

			if (ImGui::MenuItem("center", NULL, position == window_pos::center))
				position = window_pos::center;
			
			if (ImGui::MenuItem("top-left", NULL, position == window_pos::top_left))
				position = window_pos::top_left;
			
			if (ImGui::MenuItem("top-right", NULL, position == window_pos::top_right))
				position = window_pos::top_right;
			
			if (ImGui::MenuItem("bottom-left", NULL, position== window_pos::bottom_left))
				position = window_pos::bottom_left;
			
			if (ImGui::MenuItem("bottom-right", NULL, position == window_pos::bottom_right))
				position = window_pos::bottom_right;
			
			if (show_window && ImGui::MenuItem("close"))
				show_window = false;
			ImGui::EndPopup();
		}
	}


	void progressbar_with_text(const char* lable, const char* progress_bar_text, f32 percent, f32 lable_size, f32 progressbar_size_x, f32 progressbar_size_y) {

		PFF_PROFILE_FUNCTION();

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

}
