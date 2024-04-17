
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

	void imgui_debug_layer::on_attach() { 
		
		LOG(Trace, "attach imgui layer"); 
	
		IMGUI_CHECKVERSION();
		m_context = ImGui::CreateContext();
		application::get().get_renderer()->imgui_init();

		// Load fonts
		auto io = ImGui::GetIO();
		m_fonts["default"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Regular.ttf", m_font_size);
		m_fonts["bold"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Bold.ttf", m_font_size);
		m_fonts["italic"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Italic.ttf", m_font_size);
		
		m_fonts["regular_big"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Regular.ttf", m_big_font_size);
		m_fonts["bold_big"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Bold.ttf", m_big_font_size);
		m_fonts["italic_big"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Italic.ttf", m_big_font_size);
		
		m_fonts["giant"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Bold.ttf", 30.f);
		io.FontDefault = m_fonts["default"];
		
		// execute a gpu command to upload imgui font textures
		application::get().get_renderer()->imgui_create_fonts();
	}

	void imgui_debug_layer::on_detach() { 
		
		LOG(Trace, "detach imgui layer");

		application::get().get_renderer()->imgui_shutdown();
		ImGui::DestroyContext(m_context);
	}



	void imgui_debug_layer::on_update(const f32 delta_time) {

		if (m_show_FPS_window)
			application::get().get_fps_values(m_limit_fps, m_target_fps, m_current_fps, m_work_time, m_sleep_time);
	}

	void imgui_debug_layer::on_event(event& event) { }

	void imgui_debug_layer::on_imgui_render() {

		PFF_PROFILE_FUNCTION();
		ImGui::SetCurrentContext(m_context);

		ImGui::ShowDemoWindow();

		if (m_show_FPS_window)
			show_FPS();
	}

	void imgui_debug_layer::show_FPS() {

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

			if (m_show_FPS_window) {

				PFF::UI::imgui::util::set_next_window_pos(location);
				ImGui::SetNextWindowBgAlpha(0.8f); // Transparent background
				if (ImGui::Begin("performance_timer", &m_show_FPS_window, window_flags)) {

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


					PFF::UI::imgui::util::next_window_position_selector(location, m_show_FPS_window);
				}
				ImGui::End();
			}
		}
	}

	ImFont* imgui_debug_layer::get_font(const std::string& name) {

		return nullptr;
	}

}
