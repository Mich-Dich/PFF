
#include "util/pffpch.h"

#include <imgui.h>

#include "layer.h"
#include "application.h"
#include "engine/render/renderer.h"
#include "util/imgui/imgui_util.h"

#include "imgui_layer.h"

#define PFF_UI_ACTIVE_THEME				PFF::UI::THEME::current_theme

#define LERP_MAIN_COLOR_DARK(value)			{main_color.x * value, main_color.y * value, main_color.z * value, 1.f }		// Set [w] to be [1.f] to disable accidental transparency
#define LERP_MAIN_COLOR_LIGHT(value)		{	(1.f - value) * 1.f + value * main_color.x, \
												(1.f - value) * 1.f + value * main_color.y, \
												(1.f - value) * 1.f + value * main_color.z, \
												1.f }																		// Set [w] to be [1.f] to disable accidental transparency

#define LERP_GRAY(value)				{value, value, value, 1.f }
#define LERP_GRAY_A(value, alpha)		{value, value, value, alpha }
#define COLOR_INT_CONVERTION(color)		IM_COL32(255 * color.x, 255 * color.y, 255 * color.z, 255 * color.w)

namespace PFF::UI {
	
	imgui_layer::imgui_layer() : layer("imgui_layer") { 
		
		CORE_LOG_INIT(); 

		main_color = { .03f,	.72f,	.0f,	1.f };
		enable_window_forder = false;
		highlited_window_bg = LERP_GRAY(0.17f);
		main_titlebar_color = LERP_MAIN_COLOR_DARK(.5f);
		default_item_width = 200.f;

		action_color_00_default = LERP_MAIN_COLOR_DARK(0.7f);
		action_color_00_hover = LERP_MAIN_COLOR_DARK(0.85f);
		action_color_00_active = LERP_MAIN_COLOR_DARK(1.f);

		action_color_gray_default = LERP_GRAY(0.2f);
		action_color_gray_hover = LERP_GRAY(0.27f);
		action_color_gray_active = LERP_GRAY(0.35f);


	}

	imgui_layer::~imgui_layer() { 
		
		CORE_LOG_SHUTDOWN();
	}

	void imgui_layer::on_attach() { 
		
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

		update_UI_theme();
		//serialize(serializer::option::load_from_file);
	}

	void imgui_layer::on_detach() { 
		
		LOG(Trace, "detach imgui layer");

		serialize(serializer::option::save_to_file);
		application::get().get_renderer()->imgui_shutdown();
		ImGui::DestroyContext(m_context);
	}



	void imgui_layer::on_update(const f32 delta_time) {

		if (m_show_FPS_window)
			application::get().get_fps_values(m_limit_fps, m_target_fps, m_current_fps, m_work_time, m_sleep_time);
	}

	void imgui_layer::on_event(event& event) { }

	void imgui_layer::on_imgui_render() {

		PFF_PROFILE_FUNCTION();
		ImGui::SetCurrentContext(m_context);

		if (m_show_FPS_window)
			show_FPS();
	}

	void imgui_layer::show_FPS() {

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

	ImFont* imgui_layer::get_font(const std::string& name) {

		return nullptr;
	}

	void imgui_layer::serialize(serializer::option option) {

		serializer::yaml(config::get_filepath_from_configtype(config::file::ui), "theme", option)

			.entry(KEY_VALUE(m_font_size))
			.entry(KEY_VALUE(m_big_font_size))
			.entry(KEY_VALUE(UI_theme))
			.entry(KEY_VALUE(main_color))
			.entry(KEY_VALUE(enable_window_forder))
			.entry(KEY_VALUE(highlited_window_bg))
			.entry(KEY_VALUE(main_titlebar_color))
			.entry(KEY_VALUE(default_item_width))

			// color heightlight
			.entry(KEY_VALUE(action_color_00_default))
			.entry(KEY_VALUE(action_color_00_hover))
			.entry(KEY_VALUE(action_color_00_active))

			// gray
			.entry(KEY_VALUE(action_color_gray_default))
			.entry(KEY_VALUE(action_color_gray_hover))
			.entry(KEY_VALUE(action_color_gray_active));
	}

	void enable_window_border(bool enable) {

		enable_window_forder = enable;
		//serialize(serializer::option::save_to_file);

		ImGuiStyle* style = &ImGui::GetStyle();
		style->WindowBorderSize = enable ? 1.f : 0.f;
	}

	void update_UI_theme() {

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		// main sizes
		style->WindowPadding = ImVec2(10.f, 4.f);
		style->FramePadding = ImVec2(4.f, 4.f);
		style->CellPadding = ImVec2(4.f, 4.f);
		style->ItemSpacing = ImVec2(4.f, 4.f);
		style->ItemInnerSpacing = ImVec2(4.f, 4.f);
		style->TouchExtraPadding = ImVec2(4.f, 4.f);
		style->IndentSpacing = 10.f;
		style->ScrollbarSize = 14.f;
		style->GrabMinSize = 14.f;

		// border
		style->WindowBorderSize = 1.0f;
		style->ChildBorderSize = 0.0f;
		style->PopupBorderSize = 0.0f;
		style->FrameBorderSize = 0.0f;
		style->TabBorderSize = 0.0f;

		// padding
		style->WindowRounding = 2.f;
		style->ChildRounding = 2.f;
		style->FrameRounding = 2.f;
		style->PopupRounding = 2.f;
		style->ScrollbarRounding = 2.f;
		style->GrabRounding = 2.f;
		style->TabRounding = 2.f;

		switch (UI_theme) {
		case PFF::UI::theme_selection::dark:
		{

			action_color_00_default = LERP_MAIN_COLOR_DARK(.7f);
			action_color_00_hover = LERP_MAIN_COLOR_DARK(.85f);
			action_color_00_active = LERP_MAIN_COLOR_DARK(1.f);

			action_color_gray_default = LERP_GRAY(0.25f);
			action_color_gray_hover = LERP_GRAY(0.3f);
			action_color_gray_active = LERP_GRAY(0.35f);

			highlited_window_bg = LERP_GRAY(0.17f);
			main_titlebar_color = LERP_MAIN_COLOR_DARK(.5f);

			colors[ImGuiCol_Text] = LERP_GRAY(1.f);
			colors[ImGuiCol_TextDisabled] = LERP_GRAY(.7f);
			colors[ImGuiCol_WindowBg] = LERP_GRAY(.14f);
			colors[ImGuiCol_ChildBg] = LERP_GRAY(.14f);
			colors[ImGuiCol_PopupBg] = LERP_GRAY(.12f);
			colors[ImGuiCol_Border] = LERP_GRAY_A(.43f, .5f);
			colors[ImGuiCol_BorderShadow] = LERP_GRAY_A(.12f, .5f);
			colors[ImGuiCol_FrameBg] = LERP_GRAY_A(.06f, .54f);
			colors[ImGuiCol_FrameBgHovered] = LERP_GRAY_A(.19f, .4f);
			colors[ImGuiCol_FrameBgActive] = LERP_GRAY_A(.3f, .67f);
			colors[ImGuiCol_TitleBg] = action_color_gray_default;
			colors[ImGuiCol_TitleBgActive] = action_color_gray_active;
			colors[ImGuiCol_TitleBgCollapsed] = action_color_gray_default;
			colors[ImGuiCol_MenuBarBg] = LERP_GRAY(.1f);
			colors[ImGuiCol_ScrollbarBg] = LERP_GRAY(0.23f);
			colors[ImGuiCol_ScrollbarGrab] = action_color_00_default;
			colors[ImGuiCol_ScrollbarGrabHovered] = action_color_00_hover;
			colors[ImGuiCol_ScrollbarGrabActive] = action_color_00_active;
			colors[ImGuiCol_CheckMark] = action_color_00_active;
			colors[ImGuiCol_SliderGrab] = action_color_00_default;
			colors[ImGuiCol_SliderGrabActive] = action_color_00_active;
			colors[ImGuiCol_Button] = action_color_00_default;
			colors[ImGuiCol_ButtonHovered] = action_color_00_hover;
			colors[ImGuiCol_ButtonActive] = action_color_00_active;
			colors[ImGuiCol_Header] = LERP_GRAY(.3f);
			colors[ImGuiCol_HeaderHovered] = LERP_GRAY(.4f);
			colors[ImGuiCol_HeaderActive] = LERP_GRAY(.5f);
			colors[ImGuiCol_Separator] = LERP_GRAY(.45f);
			colors[ImGuiCol_SeparatorHovered] = LERP_GRAY(.45f);
			colors[ImGuiCol_SeparatorActive] = LERP_GRAY(.45f);
			colors[ImGuiCol_ResizeGrip] = action_color_00_default;
			colors[ImGuiCol_ResizeGripHovered] = action_color_00_hover;
			colors[ImGuiCol_ResizeGripActive] = action_color_00_active;
			colors[ImGuiCol_Tab] = action_color_00_default;
			colors[ImGuiCol_TabHovered] = action_color_00_hover;
			colors[ImGuiCol_TabActive] = action_color_00_active;
			colors[ImGuiCol_TabUnfocused] = LERP_MAIN_COLOR_DARK(0.5f);
			colors[ImGuiCol_TabUnfocusedActive] = LERP_MAIN_COLOR_DARK(0.6f);
			colors[ImGuiCol_DockingPreview] = action_color_00_active;
			colors[ImGuiCol_DockingEmptyBg] = LERP_GRAY(0.2f);
			colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = LERP_MAIN_COLOR_DARK(.75f);
			colors[ImGuiCol_PlotHistogramHovered] = action_color_00_active;
			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
			colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
			colors[ImGuiCol_TextSelectedBg] = LERP_MAIN_COLOR_DARK(.4f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		} break;

		case PFF::UI::theme_selection::light:
		{

			action_color_00_default = LERP_MAIN_COLOR_LIGHT(0.7f);
			action_color_00_hover = LERP_MAIN_COLOR_LIGHT(0.85f);
			action_color_00_active = LERP_MAIN_COLOR_LIGHT(1.f);

			action_color_gray_default = LERP_GRAY(0.2f);
			action_color_gray_hover = LERP_GRAY(0.27f);
			action_color_gray_active = LERP_GRAY(0.35f);

			highlited_window_bg = LERP_GRAY(0.8f);
			main_titlebar_color = LERP_MAIN_COLOR_LIGHT(.5f);

			colors[ImGuiCol_Text] = LERP_GRAY(.0f);
			colors[ImGuiCol_TextDisabled] = LERP_GRAY(.3f);
			colors[ImGuiCol_WindowBg] = LERP_GRAY(.85f);
			colors[ImGuiCol_ChildBg] = LERP_GRAY(.85f);
			colors[ImGuiCol_PopupBg] = LERP_GRAY(.8f);
			colors[ImGuiCol_Border] = LERP_GRAY_A(0.2f, .50f);
			colors[ImGuiCol_BorderShadow] = LERP_GRAY(.12f);
			colors[ImGuiCol_FrameBg] = LERP_GRAY_A(.75f, .75f);
			colors[ImGuiCol_FrameBgHovered] = LERP_GRAY_A(.70f, .75f);
			colors[ImGuiCol_FrameBgActive] = LERP_GRAY_A(.65f, .75f);
			colors[ImGuiCol_TitleBg] = action_color_00_default;
			colors[ImGuiCol_TitleBgActive] = action_color_00_active;
			colors[ImGuiCol_TitleBgCollapsed] = action_color_00_default;
			colors[ImGuiCol_MenuBarBg] = LERP_GRAY(.58f);
			colors[ImGuiCol_ScrollbarBg] = LERP_GRAY(.75f);
			colors[ImGuiCol_ScrollbarGrab] = action_color_00_default;
			colors[ImGuiCol_ScrollbarGrabHovered] = action_color_00_hover;
			colors[ImGuiCol_ScrollbarGrabActive] = action_color_00_active;
			colors[ImGuiCol_CheckMark] = action_color_00_active;
			colors[ImGuiCol_SliderGrab] = action_color_00_default;
			colors[ImGuiCol_SliderGrabActive] = action_color_00_active;
			colors[ImGuiCol_Button] = action_color_00_default;
			colors[ImGuiCol_ButtonHovered] = action_color_00_hover;
			colors[ImGuiCol_ButtonActive] = action_color_00_active;
			colors[ImGuiCol_Header] = LERP_GRAY(.75f);
			colors[ImGuiCol_HeaderHovered] = LERP_GRAY(.7f);
			colors[ImGuiCol_HeaderActive] = LERP_GRAY(.65f);
			colors[ImGuiCol_Separator] = LERP_GRAY(.45f);
			colors[ImGuiCol_SeparatorHovered] = LERP_GRAY(.45f);
			colors[ImGuiCol_SeparatorActive] = LERP_GRAY(.45f);
			colors[ImGuiCol_ResizeGrip] = action_color_00_default;
			colors[ImGuiCol_ResizeGripHovered] = action_color_00_hover;
			colors[ImGuiCol_ResizeGripActive] = action_color_00_active;
			colors[ImGuiCol_Tab] = action_color_00_default;
			colors[ImGuiCol_TabHovered] = action_color_00_hover;
			colors[ImGuiCol_TabActive] = action_color_00_active;
			colors[ImGuiCol_TabUnfocused] = LERP_MAIN_COLOR_LIGHT(0.5f);
			colors[ImGuiCol_TabUnfocusedActive] = LERP_MAIN_COLOR_LIGHT(0.6f);
			colors[ImGuiCol_DockingPreview] = action_color_00_active;
			colors[ImGuiCol_DockingEmptyBg] = LERP_GRAY(.2f);
			colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = LERP_MAIN_COLOR_LIGHT(.75f);
			colors[ImGuiCol_PlotHistogramHovered] = action_color_00_active;
			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
			colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
			colors[ImGuiCol_TextSelectedBg] = LERP_MAIN_COLOR_LIGHT(.4f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		} break;

		default:
			break;
		}

		//serialize(serializer::option::load_from_file);
	}

	void update_UI_colors(ImVec4 new_color) {

		main_color = new_color;
		//serialize(serializer::option::save_to_file);
		update_UI_theme();
	}
}
