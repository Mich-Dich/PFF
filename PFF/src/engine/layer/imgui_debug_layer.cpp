
#include "util/pffpch.h"

#include <imgui.h>

#include "layer.h"
#include "application.h"
#include "engine/render/renderer.h"
#include "util/color_theme.h"
#include "util/imgui/imgui_util.h"

#include "imgui_debug_layer.h"

namespace PFF {
	

	imgui_debug_layer::imgui_debug_layer() : layer("imgui_debug_layer") { CORE_LOG_INIT(); }

	imgui_debug_layer::~imgui_debug_layer() { CORE_LOG_SHUTDOWN(); }

	void imgui_debug_layer::on_attach() { LOG(Trace, "attach imgui layer"); }

	void imgui_debug_layer::on_detach() { LOG(Trace, "detach imgui layer"); }



	void imgui_debug_layer::on_update(const f32 delta_time) {

		if (m_show_performance_window)
			application::get().get_fps_values(m_limit_fps, m_target_fps, m_current_fps, m_work_time, m_sleep_time);
	}

	void imgui_debug_layer::on_event(event& event) { }

	void imgui_debug_layer::on_imgui_render() {

		PFF_PROFILE_FUNCTION();

		ImGui::ShowDemoWindow();

		if (m_show_performance_window)
			show_performance();
	}

	void imgui_debug_layer::show_performance() {

		static UI::imgui::util::window_pos location = UI::imgui::util::window_pos::top_right;
		{
			ImGuiWindowFlags window_flags = (
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoDocking |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_NoNav);
			if (location != UI::imgui::util::window_pos::custom)
				window_flags |= ImGuiWindowFlags_NoMove;

			if (m_show_performance_window) {

				PFF::UI::imgui::util::set_next_window_pos(location);
				ImGui::SetNextWindowBgAlpha(0.8f); // Transparent background
				if (ImGui::Begin("performance_timer", &m_show_performance_window, window_flags)) {

					// Get the current style
					ImGuiStyle& style = ImGui::GetStyle();

					// Get the line spacing(vertical padding around text)
					f32 lineSpacing = style.ItemSpacing.y / 2;
					f32 fontSize = ImGui::GetFontSize();
					f32 work_percent = static_cast<f32>(m_work_time / (m_work_time + m_sleep_time));
					f32 sleep_percent = 1 - work_percent;
					char formattedText[32];
					ImVec2 curser_pos;
					ImVec2 textSize;

					ImGui::Text("performance timer");
					ImGui::Separator();
					if (m_limit_fps)
						ImGui::Text("current fps    %4d/%4d", m_current_fps, m_target_fps);
					else
						ImGui::Text("current fps    %4d", m_current_fps);

					// work_time
					snprintf(formattedText, sizeof(formattedText), "%5.2f ms", m_work_time);
					PFF::UI::imgui::util::progressbar_with_text("work time:", formattedText, work_percent, 70.f, 70.f);

					//sleep time
					snprintf(formattedText, sizeof(formattedText), "%5.2f ms", m_sleep_time);
					PFF::UI::imgui::util::progressbar_with_text("sleep time ", formattedText, sleep_percent, 70.f, 70.f);


					PFF::UI::imgui::util::next_window_position_selector(location, m_show_performance_window);
				}
				ImGui::End();
			}
		}
	}


}
