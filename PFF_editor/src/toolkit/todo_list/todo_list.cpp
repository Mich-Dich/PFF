
#include <util/pch_editor.h>

#include <imgui.h>

//#include "util/util_editor.h"
//#include "util/ui/panels/pannel_collection.h"
//#include "engine/color_theme.h"

#include "todo_list.h"

namespace PFF::toolkit {

	/*
	void window_todo_list() {

		if (!m_show_todo_list)
			return;

		ImGuiStyle* style = &ImGui::GetStyle();
		ImGuiWindowFlags window_flags{};
		const ImVec2 window_padding = { style->WindowPadding.x + 10.f, style->WindowPadding.y + 35.f };

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 500, viewport->Size.y - 300), ImGuiCond_Appearing);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (ImGui::Begin("ToDo List", &m_show_todo_list, window_flags)) {

			const f32 first_width = 250.f;
			UI::custom_frame(first_width, [&first_width] {

				UI::shift_cursor_pos(30, 0);
				UI::draw_big_text("Topics");

				const char* items[] = { "General_settings", "General_settings 02", "General_settings 03" };
				static u32 item_current_idx = 0;

				ImGuiStyle* style = &ImGui::GetStyle();
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(25, style->FramePadding.y));
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(.5f, .5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, PFF::UI::THEME::highlited_window_bg);
				if (ImGui::BeginListBox("##Topic_selector", ImVec2(first_width, (ARRAY_SIZE(items) * ImGui::GetTextLineHeightWithSpacing()) - 1))) {

					for (u32 n = 0; n < ARRAY_SIZE(items); n++) {

						const bool is_selected = (item_current_idx == n);
						if (ImGui::Selectable(items[n], is_selected))
							item_current_idx = n;
					}
					ImGui::EndListBox();

				}
				ImGui::PopStyleColor();
				ImGui::PopStyleVar(3);

				}
				, [] {

					if (ImGui::CollapsingHeader("Open Items", ImGuiTreeNodeFlags_DefaultOpen)) {

						for (u32 x = 0; x < 3; x++) {

							bool test = false;

							ImGui::Checkbox("##xx", &test);
							ImGui::SameLine();
							UI::draw_big_text("Test Title");
							ImGui::Text("test description of todo item");
						}
					}

					ImGui::Spacing();

					if (ImGui::CollapsingHeader("done Items")) {
						for (u32 x = 0; x < 3; x++) {

							bool test = false;
							ImGui::Checkbox("##xx", &test);

							ImGui::SameLine();
							ImGui::BeginGroup();

							UI::draw_big_text("Test Title");
							ImGui::Text("test description of todo item");

							ImGui::EndGroup();

						}
					}

					});

		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
	*/

}

