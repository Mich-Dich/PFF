#pragma once

#include "PFF_editor.h"

namespace PFF::settings {
	
	void window_editor_settings(bool* show) {

		if (!(*show))
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (!ImGui::Begin("Editor Settings", show, 0)) {

			ImGui::End();
			ImGui::PopStyleVar();
			return;
		}

		const f32 default_item_width = 250;
		const f32 first_width = 250.f;
		static u32 item_current_idx = 0;

		UI::custom_frame_NEW(first_width, false, ImGui::ColorConvertFloat4ToU32(UI::get_default_gray_1_ref()), [first_width]() mutable {

			const char* items[] = { "Displaying Components" };

			ImGuiStyle* style = &ImGui::GetStyle();
			f32 touch_extra_padding_y_buffer = style->TouchExtraPadding.y;
			style->TouchExtraPadding.y = 0;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(25, style->FramePadding.y));
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(.5f, .5f));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(ImVec4(0)));
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
			style->TouchExtraPadding.y = touch_extra_padding_y_buffer;

		}, [&] {

			if (item_current_idx == 0) {		// [Displaying Components] in array [items]

				UI::begin_table("##display_components", false);

				UI::table_row("display rotator in degrees", PFF_editor::get().get_editor_settings_ref().display_rotator_in_degrees);

				UI::end_table();
			}

		});

		ImGui::End();
		ImGui::PopStyleVar();
	}

}
