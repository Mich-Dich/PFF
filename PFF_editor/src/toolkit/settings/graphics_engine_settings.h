#pragma once

#include "util/ui/pannel_collection.h"

namespace PFF::toolkit::settings {

	static bool s_show_graphics_engine_settings = false;

	static const u8 number_of_present_modes = 6;
	static const char* present_modes[number_of_present_modes] = { "Immediate", "Mailbox", "FiFo", "Relaxed FiFo", "Shared Demand Refresh", "Shared Continuous Refresh" };

	void window_graphics_engine() {

		if (!s_show_graphics_engine_settings)
			return;

		ImVec2 topic_button_size = { 150, 30 };

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 500, viewport->Size.y - 300), ImGuiCond_Once);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (ImGui::Begin("Graphics Engine Settings", &s_show_graphics_engine_settings, window_flags)) {

			const f32 default_item_width = 250;
			const f32 first_width = 250.f;
			static u32 item_current_idx = 0;
			UI::custom_frame(first_width, [first_width]() mutable {

				const char* items[] = { "General_settings", "General_settings 02", "General_settings 03" };

				ImGuiStyle* style = &ImGui::GetStyle();
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(25, style->FramePadding.y));
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(.5f, .5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, UI::highlited_window_bg);
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

			}, [&] {

				if (item_current_idx == 0) {		// [General_settings] in array [items]

					ImGui::BeginGroup();
					static u32 present_mode_selected = 0;
					if (ImGui::CollapsingHeader("Swapchain##GraphicsSettings", ImGuiTreeNodeFlags_DefaultOpen)) {

						static int selected = 1;

						//ImGui::ShowDemoWindow();

						UI::begin_table("##graphics_settings_table", false);

						UI::table_row([=] {

							if (ImGui::Button(" ? ##Present_mode_popup"))
								ImGui::OpenPopup("present_mode_explanations");

							if (ImGui::BeginPopup("present_mode_explanations")) {

								ImGui::Spacing();
								ImGui::SeparatorText("Immediate");
								ImGui::Indent(10.f);
								ImGui::Text("The presentation engine does not wait for a vertical blanking period to update the current image,\n"
									"meaning this mode may result in visible tearing. No internal queuing of presentation requests is needed,\n"
									"as the requests are applied immediately.");
								ImGui::Indent(-10.f);

								ImGui::Spacing();
								ImGui::SeparatorText("Mailbox");
								ImGui::Indent(10.f);
								ImGui::Text("The presentation engine waits for the next vertical blanking period to update the current image,\n"
									"preventing tearing. It maintains a single-entry queue for pending presentation requests.\n"
									"If the queue is full, new requests replace existing ones, ensuring timely processing during each vertical blanking period.");
								ImGui::Indent(-10.f);

								ImGui::Spacing();
								ImGui::SeparatorText("FiFo (First-In, First-Out)");
								ImGui::Indent(10.f);
								ImGui::Text("Similar to Mailbox, the presentation engine waits for the next vertical blanking period to update the current image,\n"
									"avoiding tearing. Pending presentation requests are queued, with new requests added to the end and processed in order\n"
									"during each non-empty blanking period.");
								ImGui::Indent(-10.f);

								ImGui::Spacing();
								ImGui::SeparatorText("Relaxed FiFo");
								ImGui::Indent(10.f);
								ImGui::Text("The presentation engine typically waits for the next vertical blanking period to update the current image.\n"
									"However, if a blanking period has passed since the last update, it may update immediately, potentially resulting in tearing.\n"
									"It uses a queue for pending presentation requests, ensuring timely processing.");
								ImGui::Indent(-10.f);

								ImGui::Spacing();
								ImGui::SeparatorText("Shared Demand Refresh");
								ImGui::Indent(10.f);
								ImGui::Text("The presentation engine and application share access to a single image.\n"
									"The engine updates the image only after receiving a presentation request,\n"
									"while the application must request updates as needed. Tearing may occur since updates can happen at any point.");
								ImGui::Indent(-10.f);

								ImGui::Spacing();
								ImGui::SeparatorText("Shared Continuous Refresh");
								ImGui::Indent(10.f);
								ImGui::Text("Both the presentation engine and application have access to a shared image.\n"
									"The engine periodically updates the image on its regular refresh cycle without needing additional requests from the application.\n"
									"However, if rendering is not timed correctly, tearing may occur.");
								ImGui::Indent(-10.f);
								ImGui::Spacing();

								ImGui::EndPopup();
							}

							ImGui::SameLine();
							ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
							if (ImGui::Combo("##Present_mode_selector", &selected, present_modes, number_of_present_modes)) {

								LOG(Error, "NOT IMPLEMENTED YET" << selected);
							}

							}, [] { ImGui::Text("Choose swapchain present mode of the [swapchain] for optimal performance and visual quality"); });

						UI::end_table();

					}



					ImGui::EndGroup();
				}

				if (item_current_idx == 0) {		// [General_settings] in array [items]

				}

			});
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}

}
