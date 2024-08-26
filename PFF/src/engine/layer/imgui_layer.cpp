
#include "util/pffpch.h"

#include <imgui.h>
#include <ImGuizmo.h>

//#include "engine/render/image.h"
#include "util/ui/window_images.embed"

#include "layer.h"
#include "application.h"
#include "engine/render/renderer.h"
#include "util/ui/pannel_collection.h"

#include "imgui_layer.h"


#define PFF_UI_ACTIVE_THEME					PFF::UI::THEME::current_theme

#define LERP_GRAY_A(value, alpha)			{value, value, value, alpha }
//#define COLOR_INT_CONVERTION(color)			IM_COL32(255 * color.x, 255 * color.y, 255 * color.z, 255 * color.w)
#define LERP_MAIN_COLOR_DARK(value)			{main_color.x * value, main_color.y * value, main_color.z * value, 1.f }		// Set [w] to be [1.f] to disable accidental transparency
#define LERP_MAIN_COLOR_LIGHT(value)		{	(1.f - value) * 1.f + value * main_color.x, \
												(1.f - value) * 1.f + value * main_color.y, \
												(1.f - value) * 1.f + value * main_color.z, \
												1.f }																		// Set [w] to be [1.f] to disable accidental transparency


namespace PFF::UI {

	static ImVec4 vector_multi(const ImVec4& vec_0, const ImVec4& vec_1) {
		return ImVec4{ vec_0.x * vec_1.x, vec_0.y * vec_1.y, vec_0.z * vec_1.z, vec_0.w * vec_1.w };
	}

	void set_UI_theme_selection(theme_selection theme_selection) { UI_theme = theme_selection; }

	void imgui_layer::serialize(serializer::option option) {

		serializer::yaml(config::get_filepath_from_configtype(application::get().get_project_path(), config::file::ui), "performance_display", option)
			.entry(KEY_VALUE(m_show_FPS_window))
			.entry(KEY_VALUE(m_show_renderer_metrik))
			.entry(KEY_VALUE(renderer_metrik_window_location))
			.entry(KEY_VALUE(FPS_window_location));
	}

	static void serialize(serializer::option option) {

		serializer::yaml(config::get_filepath_from_configtype(application::get().get_project_path(), config::file::ui), "theme", option)
			.entry(KEY_VALUE(m_font_size))
			.entry(KEY_VALUE(m_font_size_header_0))
			.entry(KEY_VALUE(m_font_size_header_1))
			.entry(KEY_VALUE(m_font_size_header_2))
			.entry(KEY_VALUE(m_big_font_size))
			.entry(KEY_VALUE(UI_theme))
			.entry(KEY_VALUE(enable_window_forder))
			.entry(KEY_VALUE(highlited_window_bg))
			.entry(KEY_VALUE(default_item_width))

			// color
			.entry(KEY_VALUE(main_color))
			.entry(KEY_VALUE(main_titlebar_color))

			// color heightlight
			.entry(KEY_VALUE(action_color_00_faded))
			.entry(KEY_VALUE(action_color_00_weak))
			.entry(KEY_VALUE(action_color_00_default))
			.entry(KEY_VALUE(action_color_00_hover))
			.entry(KEY_VALUE(action_color_00_active))

			// gray
			.entry(KEY_VALUE(action_color_gray_default))
			.entry(KEY_VALUE(action_color_gray_hover))
			.entry(KEY_VALUE(action_color_gray_active));
		
	}

	void save_UI_theme_data() {

		serialize(serializer::option::save_to_file);
		update_UI_theme();
	}

	void load_UI_data() {

		serialize(serializer::option::load_from_file);
		update_UI_theme();
	}

	void update_UI_colors(ImVec4 new_color) {

		main_color = new_color;
		serialize(serializer::option::save_to_file);
		update_UI_theme();
	}

	imgui_layer::imgui_layer() : layer("imgui_layer") { 
		
		CORE_LOG_INIT(); 
		
		main_color = { .0f,	.4088f,	1.0f,	1.f };
		enable_window_forder = false;
		highlited_window_bg = LERP_GRAY(0.57f);
		main_titlebar_color = LERP_MAIN_COLOR_DARK(.5f);
		default_item_width = 200.f;

		action_color_00_faded = LERP_MAIN_COLOR_DARK(0.5f);
		action_color_00_weak = LERP_MAIN_COLOR_DARK(0.6f);
		action_color_00_default = LERP_MAIN_COLOR_DARK(0.7f);
		action_color_00_hover = LERP_MAIN_COLOR_DARK(0.85f);
		action_color_00_active = LERP_MAIN_COLOR_DARK(1.f);

		//{
		//	u32 w, h;
		//	void* data = image::decode(g_WindowMaximizeIcon, sizeof(g_WindowMaximizeIcon), w, h);
		//	image test_image = image(data, w, h);

		//	//test_image.reset();
		//}
	}

	imgui_layer::~imgui_layer() { 

		GET_RENDERER.imgui_shutdown();
		ImGui::DestroyContext(m_context);
		CORE_LOG_SHUTDOWN();
	}


	void imgui_layer::on_attach() { 

		LOG(Trace, "attach imgui layer"); 
	
		IMGUI_CHECKVERSION();
		m_context = ImGui::CreateContext();
		GET_RENDERER.imgui_init();
		
		serialize(serializer::option::load_from_file);

		std::filesystem::path base_path = PFF::util::get_executable_path() / std::filesystem::path("..") / "PFF" / "assets" / "fonts";
		std::filesystem::path OpenSans_path = base_path / "Open_Sans" / "static";
		std::filesystem::path Inconsolata_path = base_path / "Inconsolata" / "static";

		// Load fonts
		auto io = ImGui::GetIO();
		io.FontAllowUserScaling = true;
		m_fonts["regular"] =		io.Fonts->AddFontFromFileTTF((OpenSans_path/ "OpenSans-Regular.ttf").string().c_str(), m_font_size);
		m_fonts["bold"] =			io.Fonts->AddFontFromFileTTF((OpenSans_path/ "OpenSans-Bold.ttf").string().c_str(), m_font_size);
		m_fonts["italic"] =			io.Fonts->AddFontFromFileTTF((OpenSans_path/ "OpenSans-Italic.ttf").string().c_str(), m_font_size);

		m_fonts["regular_big"] =	io.Fonts->AddFontFromFileTTF((OpenSans_path / "OpenSans-Regular.ttf").string().c_str(), m_big_font_size);
		m_fonts["bold_big"] =		io.Fonts->AddFontFromFileTTF((OpenSans_path / "OpenSans-Bold.ttf").string().c_str(), m_big_font_size);
		m_fonts["italic_big"] =		io.Fonts->AddFontFromFileTTF((OpenSans_path / "OpenSans-Italic.ttf").string().c_str(), m_big_font_size);

		m_fonts["header_0"] =		io.Fonts->AddFontFromFileTTF((OpenSans_path / "OpenSans-Regular.ttf").string().c_str(), m_font_size_header_2);
		m_fonts["header_1"] =		io.Fonts->AddFontFromFileTTF((OpenSans_path / "OpenSans-Regular.ttf").string().c_str(), m_font_size_header_1);
		m_fonts["header_2"] =		io.Fonts->AddFontFromFileTTF((OpenSans_path / "OpenSans-Regular.ttf").string().c_str(), m_font_size_header_0);

		m_fonts["giant"] =			io.Fonts->AddFontFromFileTTF((OpenSans_path / "OpenSans-Bold.ttf").string().c_str(), 30.f);

		// C:\CustomGameEngine\PFF\PFF\assets\fonts\Inconsolata\static
		//Inconsolata-Regular
		m_fonts["monospace_regular"] = io.Fonts->AddFontFromFileTTF((Inconsolata_path / "Inconsolata-Regular.ttf").string().c_str(), m_font_size * 0.92f);
		m_fonts["monospace_regular_big"] = io.Fonts->AddFontFromFileTTF((Inconsolata_path / "Inconsolata-Regular.ttf").string().c_str(), m_big_font_size * 1.92f);

		io.FontDefault = m_fonts["regular"];

		GET_RENDERER.imgui_create_fonts();
		load_UI_data();

		ImGuizmo::SetOrthographic(false);				// currently only using perspektiv
		ImGuizmo::AllowAxisFlip(false);
	}


	void imgui_layer::on_detach() { 
		
		LOG(Trace, "detach imgui layer");

		serialize(serializer::option::save_to_file);
	}


	void imgui_layer::on_update(const f32 delta_time) {

		if (m_show_FPS_window)
			application::get().get_fps_values(m_limit_fps, m_target_fps, m_current_fps, m_work_time, m_sleep_time);
	}


	void imgui_layer::on_event(event& event) { }


	void imgui_layer::on_imgui_render() {

		PFF_PROFILE_FUNCTION();
		ImGui::SetCurrentContext(m_context);
	}


	void imgui_layer::show_renderer_metrik() {

#ifdef PFF_DEBUG								// renderer metrik is only collected in debug mode
		if (!m_show_renderer_metrik)
			return;

		static const u32 const_array_size = sizeof(f32) * 100;
		static bool show_graphs = true;

		ImGuiWindowFlags window_flags = (
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav);
		if (renderer_metrik_window_location != UI::window_pos::custom)
			window_flags |= ImGuiWindowFlags_NoMove;

		static char formatted_text[32];

		UI::set_next_window_pos_in_window(renderer_metrik_window_location);
		ImGui::SetNextWindowBgAlpha(0.8f); // Transparent background
		if (ImGui::Begin("Renderer Metrik##PFF_Engine", &m_show_FPS_window, window_flags)) {

			auto* metrik = PFF::render::vulkan::vk_renderer::get().get_renderer_metrik_pointer();
			if (UI::begin_table("Performance Display", true, ImVec2(280, 0))) {

				UI::table_row_text("mesh draws", "%d", metrik->mesh_draw);
				UI::table_row_text("material binding count", "%d", metrik->material_binding_count);
				UI::table_row_text("pipline binding count", "%d", metrik->pipline_binding_count);
				UI::table_row_text("draw calls", "%d", metrik->draw_calls);
				UI::table_row_text("triangles", "%d", metrik->triangles);
				
				UI::end_table();
			}

			if (show_graphs) {

				UI::shift_cursor_pos(0, 10);

				static const auto renderer_draw_plot_col = ImVec4(0.f, 0.61f, 0.f, 1.00f);
				static const auto draw_geometry_plot_col = ImVec4(0.f, 0.9f, 1.f, 1.00f);
				static const auto waiting_idle_plot_col = ImVec4(0.9f, 0.f, 1.f, 1.00f);

				// calc the maximum value but ensuring its atleast 1.f
				const f32 plot_max_value = math::max(1.f,	math::max(math::calc_array_max(metrik->renderer_draw_time, 100), math::max(
															math::calc_array_max(metrik->draw_geometry_time, 100),
															math::calc_array_max(metrik->waiting_idle_time, 100))));

				ImVec2 cursor_pos = ImGui::GetCursorPos();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, vector_multi(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg), ImVec4{ 0, 0, 0, 0 }));

				ImGui::PushStyleColor(ImGuiCol_PlotLines, renderer_draw_plot_col);
				ImGui::PlotLines("##metrik_renderer_draw_time", metrik->renderer_draw_time, 200, metrik->current_index, (const char*)0, 0.0f, plot_max_value, ImVec2(280, 100));
				ImGui::PopStyleColor();

				ImGui::SetCursorPos(cursor_pos);
				ImGui::PushStyleColor(ImGuiCol_PlotLines, draw_geometry_plot_col);
				ImGui::PlotLines("##metrik_draw_geometry_time", metrik->draw_geometry_time, 200, metrik->current_index, (const char*)0, 0.0f, plot_max_value, ImVec2(280, 100));
				ImGui::PopStyleColor();

				ImGui::SetCursorPos(cursor_pos);
				ImGui::PushStyleColor(ImGuiCol_PlotLines, waiting_idle_plot_col);
				ImGui::PlotLines("##metrik_draw_geometry_time", metrik->waiting_idle_time, 200, metrik->current_index, (const char*)0, 0.0f, plot_max_value, ImVec2(280, 100));
				ImGui::PopStyleColor();

				ImGui::PopStyleColor();
				ImGui::PopStyleVar();

				ImVec2 plot_pos = ImGui::GetItemRectMin();
				ImVec2 plot_size = ImGui::GetItemRectSize();

				cursor_pos = ImGui::GetCursorPos();
				static const u32 offset = static_cast<u32>(ImGui::GetTextLineHeight() / 2);

	#if 1
				// Plot Lines
				static const char* const fps_text = " 0 ms 1 ms 2 ms 3 ms 4 ms 5 ms 6 ms 7 ms 8 ms 9 ms10 ms11 ms12 ms13 ms14 ms15 ms16 ms17 ms18 ms19 ms20 ms21 ms22 ms23 ms24 ms25 ms26 ms27 ms28 ms29 ms30 ms31 ms32 ms33 ms34 ms35 ms36 ms37 ms38 ms39 ms40 ms41 ms42 ms43 ms44 ms45 ms46 ms47 ms48 ms49 ms50 ms51 ms52 ms53 ms54 ms55 ms56 ms57 ms58 ms59 ms60 ms61 ms62 ms63 ms64 ms65 ms66 ms67 ms68 ms69 ms70 ms71 ms72 ms73 ms74 ms75 ms76 ms77 ms78 ms79 ms80 ms81 ms82 ms83 ms84 ms85 ms86 ms87 ms88 ms89 ms90 ms91 ms92 ms93 ms94 ms95 ms96 ms97 ms98 ms99 ms";
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				ImVec2 plot_max_pos = { plot_pos.x + plot_size.x , plot_pos.y + plot_size.y };
				static const u32 interval = (plot_max_value > 100) ? 10 : 1;
				static const u32 text_size = 5;
				static const u64 text_begin_offset = (plot_max_value > 100) ? text_size * 10 : 0;

				u32 num_of_displayed_texts = 0;
				u32 buffer = static_cast<u32>(plot_max_value / 8);
				for (u32 x = 0; x < plot_max_value - 1; x += interval) {

					if (x > (buffer) * num_of_displayed_texts) {

						const char* text = fps_text + (math::min<u64>(98, x + text_begin_offset) * text_size);
						float y = plot_max_pos.y - (x / plot_max_value * plot_size.y);
						ImU32 color = (x % 5 == 0) ? IM_COL32(action_color_00_active.x * 255, action_color_00_active.y * 255, action_color_00_active.z * 255, 255) : IM_COL32(200, 200, 200, 180);

						f32 y_pos = plot_max_pos.y - (x / plot_max_value * plot_size.y);
						draw_list->AddLine(ImVec2(plot_pos.x, y), ImVec2(plot_max_pos.x - 55, y), color);
						draw_list->AddText(ImVec2(plot_max_pos.x - 50, y - offset), IM_COL32(200, 200, 200, 180), text, text + text_size);
						draw_list->AddLine(ImVec2(plot_max_pos.x - 15, y), ImVec2(plot_max_pos.x, y), color);
						num_of_displayed_texts++;
					}
				}
	#endif

				ImGui::TextColored(renderer_draw_plot_col, "- render total %5.2f ms", metrik->renderer_draw_time[metrik->current_index]);
				ImGui::SameLine();
				ImGui::TextColored(draw_geometry_plot_col, " - drawing geometry %5.2f ms", metrik->draw_geometry_time[metrik->current_index]);

				ImGui::TextColored(waiting_idle_plot_col, " - waiting for GPU %5.2f ms", metrik->waiting_idle_time[metrik->current_index]);
			}

			if (ImGui::BeginPopupContextWindow()) {

				ImGui::Checkbox("show timing graphs", &show_graphs);

				UI::next_window_position_selector(renderer_metrik_window_location, m_show_renderer_metrik);

				ImGui::EndPopup();
			}
		}
		ImGui::End();
#endif
	}


	void imgui_layer::show_FPS() {

		if (!m_show_FPS_window)
			return;

		static auto last_update_time = std::chrono::steady_clock::now();
		auto current_time = std::chrono::steady_clock::now();

		static const std::chrono::milliseconds update_interval(50);		// Time interval (15 milliseconds)
		static const u32 array_size = 100;
		static f32 frame_times[array_size] = {};
		static u32 array_pointer = 0;

		static bool show_progress_bars = true;
		static bool show_graph = true;
		
		const f32 averagefps = math::calc_array_average(frame_times, array_size);

		// Check if the time since the last update exceeds the update interval
		if (current_time - last_update_time >= update_interval) {

			frame_times[array_pointer % array_size] = (f32)m_current_fps;
			array_pointer = (array_pointer + 1) % array_size;
			last_update_time = current_time;
		}

		ImGuiWindowFlags window_flags = (
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav);
		if (FPS_window_location != UI::window_pos::custom)
			window_flags |= ImGuiWindowFlags_NoMove;

		UI::set_next_window_pos_in_window(FPS_window_location);

		ImGui::SetNextWindowBgAlpha(0.8f); // Transparent background
		if (ImGui::Begin("Performance_display##PFF_Engine", &m_show_FPS_window, window_flags)) {

			// Get the line spacing(vertical padding around text)
			f32 lineSpacing = ImGui::GetStyle().ItemSpacing.y / 2;
			f32 fontSize = ImGui::GetFontSize();
			f32 work_percent = static_cast<f32>(m_work_time / (m_work_time + m_sleep_time));
			f32 sleep_percent = 1 - work_percent;
			char formatted_text[32];
			ImVec2 curser_pos;
			ImVec2 textSize;

			if (UI::begin_table("Performance Display", true, ImVec2(240.0f, 0),0, true, 0.35f)) {

				if (m_limit_fps)
					snprintf(formatted_text, sizeof(formatted_text), "%4d/%4d  average: %5.2f", m_current_fps, m_target_fps, averagefps);
				else 
					snprintf(formatted_text, sizeof(formatted_text), "%4d  average: %5.2f", m_current_fps, averagefps);

				UI::table_row_text("FPS", formatted_text);

				if (show_progress_bars) {
					snprintf(formatted_text, sizeof(formatted_text), "%5.2f ms", m_work_time);
					UI::table_row_progressbar("work time:", formatted_text, work_percent);

					snprintf(formatted_text, sizeof(formatted_text), "%5.2f ms", m_sleep_time);
					UI::table_row_progressbar("sleep time:", formatted_text, sleep_percent);
				}


				UI::end_table();
			}
			
			UI::shift_cursor_pos(0, 10);

			if (show_graph) {

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, vector_multi(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg), ImVec4{ 1, 1, 1, 0 }));
				ImGui::PlotLines("##frame_times", frame_times, array_size, (array_pointer % array_size), (const char*)0, 0.0f, FLT_MAX, ImVec2(0, 70));
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();

				// Plot Lines
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				ImVec2 plot_size = ImGui::GetItemRectSize();
				ImVec2 plot_pos = ImGui::GetItemRectMin();
				ImVec2 plot_max_pos = { plot_pos.x + plot_size.x , plot_pos.y + plot_size.y };
				static const char* const fps_text = "  0 10 20 30 40 50 60 70 80 90100110120130140150160170180190200210220230240250260270280290300310320330340350360370380390400410420430450460470480490500510520530540550560570580590600610620630640650660670680690700710720730740750760770780790800810820830840850860870890900910920930940950960970980990";
				static const u32 offset = static_cast<u32>(ImGui::GetTextLineHeight() / 2);
				static const u32 interval_thin = 10;
				static const u64 interval_thick = 50;


				const f32 max_value = math::calc_array_max(frame_times, 100);

				u32 num_of_displayed_texts = 0;
				u32 buffer = static_cast<u32>(max_value / 10);

				for (u64 i = 0; i <= max_value; i += interval_thin) {

					if (i > (buffer) * num_of_displayed_texts) {

						const char* text = fps_text + (math::min<u64>(98, i / static_cast<u64>(interval_thin)) * 3);
						float y = plot_max_pos.y - (i / max_value * plot_size.y);
						ImU32 color = (i % interval_thick == 0) ? IM_COL32(action_color_00_active.x * 255, action_color_00_active.y * 255, action_color_00_active.z * 255, 255) : IM_COL32(200, 200, 200, 180);

						draw_list->AddLine(ImVec2(plot_pos.x, y), ImVec2(plot_max_pos.x - 55, y), color);
						draw_list->AddText(ImVec2(plot_max_pos.x - 50, y - offset), IM_COL32(200, 200, 200, 180), text, text + 3);
						draw_list->AddLine(ImVec2(plot_max_pos.x - 20, y), ImVec2(plot_max_pos.x, y), color);
						num_of_displayed_texts++;
					}
				}
			}

			if (ImGui::BeginPopupContextWindow()) {

				static u32 min_fps = 1;
				static u32 max_fps = 1000;
				if (UI::begin_table("Performance Display", true, ImVec2(200.0f, 0))) {

					if (UI::table_row_drag_scalar<u32>("target FPS", application::get().get_target_fps_ref(), "%u FPS", min_fps, max_fps))
						application::get().set_fps_settings(true, application::get().get_target_fps());

					if (UI::table_row_drag_scalar<u32>("Non-focus FPS", application::get().get_nonefocus_fps_ref(), "%u FPS", min_fps, max_fps))
						application::get().set_fps_settings(false, application::get().get_nonefocus_fps());

					UI::end_table();
				}

				ImGui::Checkbox("show progress bars", &show_progress_bars);
				ImGui::Checkbox("show FPS graph", &show_graph);

				UI::next_window_position_selector(FPS_window_location, m_show_FPS_window);

				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}

	ImFont* imgui_layer::get_font(const std::string& name) {

		for (auto loc_font : m_fonts)
			if (loc_font.first == name)
				return loc_font.second;
	
		return nullptr;
	}

	void enable_window_border(bool enable) {

		enable_window_forder = enable;
		//serialize(serializer::option::save_to_file);

		ImGuiStyle* style = &ImGui::GetStyle();
		style->WindowBorderSize = enable ? 1.f : 0.f;
	}
	
	void update_UI_theme() {
		
		//CORE_LOG(Debug, "updating UI theme");

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
		style->WindowMenuButtonPosition = ImGuiDir_Right;

		// border
		style->WindowBorderSize = 1.0f;
		style->ChildBorderSize = 0.0f;
		style->PopupBorderSize = 0.0f;
		style->FrameBorderSize = 0.0f;
		style->TabBorderSize = 0.0f;
		style->TabBarBorderSize = 0.0f;

		// padding
		style->WindowRounding = 2.f;
		style->ChildRounding = 2.f;
		style->FrameRounding = 2.f;
		style->PopupRounding = 2.f;
		style->ScrollbarRounding = 2.f;
		style->GrabRounding = 2.f;
		style->TabRounding = 2.f;

		/*
		Alpha;                      // Global alpha applies to everything in Dear ImGui.
		float       DisabledAlpha;              // Additional alpha multiplier applied by BeginDisabled(). Multiply over current value of Alpha.
		ImVec2      WindowPadding;              // Padding within a window.
		float       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
		float       WindowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		ImVec2      WindowMinSize;              // Minimum window size. This is a global setting. If you want to constrain individual windows, use SetNextWindowSizeConstraints().
		ImVec2      WindowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
		ImGuiDir    WindowMenuButtonPosition;   // Side of the collapsing/docking button in the title bar (None/Left/Right). Defaults to ImGuiDir_Left.
		float       ChildRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
		float       ChildBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		float       PopupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
		float       PopupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		ImVec2      FramePadding;               // Padding within a framed rectangle (used by most widgets).
		float       FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
		float       FrameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		ImVec2      ItemSpacing;                // Horizontal and vertical spacing between widgets/lines.
		ImVec2      ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
		ImVec2      CellPadding;                // Padding within a table cell. Cellpadding.x is locked for entire table. CellPadding.y may be altered between different rows.
		ImVec2      TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
		float       IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
		float       ColumnsMinSpacing;          // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
		float       ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
		float       ScrollbarRounding;          // Radius of grab corners for scrollbar.
		float       GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
		float       GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
		float       LogSliderDeadzone;          // The size in pixels of the dead-zone around zero on logarithmic sliders that cross zero.
		float       TabRounding;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
		float       TabBorderSize;              // Thickness of border around tabs.
		float       TabMinWidthForCloseButton;  // Minimum width for close button to appear on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected.
		float       TabBarBorderSize;           // Thickness of tab-bar separator, which takes on the tab active color to denote focus.
		float       TableAngledHeadersAngle;    // Angle of angled headers (supported values range from -50.0f degrees to +50.0f degrees).
		ImGuiDir    ColorButtonPosition;        // Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
		ImVec2      ButtonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
		ImVec2      SelectableTextAlign;        // Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.
		float       SeparatorTextBorderSize;    // Thickkness of border in SeparatorText()
		ImVec2      SeparatorTextAlign;         // Alignment of text within the separator. Defaults to (0.0f, 0.5f) (left aligned, center).
		ImVec2      SeparatorTextPadding;       // Horizontal offset of text from each edge of the separator + spacing on other axis. Generally small values. .y is recommended to be == FramePadding.y.
		ImVec2      DisplayWindowPadding;       // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
		ImVec2      DisplaySafeAreaPadding;     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!
		float       DockingSeparatorSize;       // Thickness of resizing border between docked windows
		float       MouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). We apply per-monitor DPI scaling over this scale. May be removed later.
		bool        AntiAliasedLines;           // Enable anti-aliased lines/borders. Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).
		bool        AntiAliasedLinesUseTex;     // Enable anti-aliased lines/borders using textures where possible. Require backend to render with bilinear filtering (NOT point/nearest filtering). Latched at the beginning of the frame (copied to ImDrawList).
		bool        AntiAliasedFill;            // Enable anti-aliased edges around filled shapes (rounded rectangles, circles, etc.). Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).
		float       CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
		float       CircleTessellationMaxError; // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.
		*/
		
		switch (UI_theme) {

			case PFF::UI::theme_selection::dark: {

				action_color_00_faded = LERP_MAIN_COLOR_DARK(0.5f);
				action_color_00_weak = LERP_MAIN_COLOR_DARK(0.6f);
				action_color_00_default = LERP_MAIN_COLOR_DARK(0.7f);
				action_color_00_hover = LERP_MAIN_COLOR_DARK(0.85f);
				action_color_00_active = LERP_MAIN_COLOR_DARK(0.92f);

				action_color_gray_default = LERP_GRAY(0.15f);
				action_color_gray_hover = LERP_GRAY(0.2f);
				action_color_gray_active = LERP_GRAY(0.25f);

				highlited_window_bg = LERP_GRAY(0.57f);
				main_titlebar_color = LERP_MAIN_COLOR_DARK(.5f);

				colors[ImGuiCol_Text]					= IMCOLOR_GRAY(255);
				colors[ImGuiCol_TextDisabled]			= IMCOLOR_GRAY(180);
				colors[ImGuiCol_WindowBg]				= default_gray;
				colors[ImGuiCol_ChildBg]				= default_gray;
				colors[ImGuiCol_PopupBg]				= IMCOLOR_GRAY(20);
				colors[ImGuiCol_Border]					= LERP_GRAY_A(.43f, .5f);
				colors[ImGuiCol_BorderShadow]			= LERP_GRAY_A(.12f, .5f);
				colors[ImGuiCol_FrameBg]				= LERP_GRAY_A(.06f, .54f);
				colors[ImGuiCol_FrameBgHovered]			= LERP_GRAY_A(.19f, .4f);
				colors[ImGuiCol_FrameBgActive]			= LERP_GRAY_A(.3f, .67f);
				colors[ImGuiCol_TitleBg]				= IMCOLOR_GRAY(22);
				colors[ImGuiCol_TitleBgActive]			= IMCOLOR_GRAY(22);
				colors[ImGuiCol_TitleBgCollapsed]		= IMCOLOR_GRAY(22);
				colors[ImGuiCol_MenuBarBg]				= LERP_GRAY(.1f);
				colors[ImGuiCol_ScrollbarBg]			= LERP_GRAY(0.23f);
				colors[ImGuiCol_ScrollbarGrab]			= action_color_00_default;
				colors[ImGuiCol_ScrollbarGrabHovered]	= action_color_00_hover;
				colors[ImGuiCol_ScrollbarGrabActive]	= action_color_00_active;
				colors[ImGuiCol_CheckMark]				= action_color_00_active;
				colors[ImGuiCol_SliderGrab]				= action_color_00_default;
				colors[ImGuiCol_SliderGrabActive]		= action_color_00_active;
				colors[ImGuiCol_Button]					= action_color_00_default;
				colors[ImGuiCol_ButtonHovered]			= action_color_00_hover;
				colors[ImGuiCol_ButtonActive]			= action_color_00_active;
				colors[ImGuiCol_Header]					= LERP_GRAY(.3f);
				colors[ImGuiCol_HeaderHovered]			= LERP_GRAY(.4f);
				colors[ImGuiCol_HeaderActive]			= LERP_GRAY(.5f);
				colors[ImGuiCol_Separator]				= LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorHovered]		= LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorActive]		= LERP_GRAY(.45f);
				colors[ImGuiCol_ResizeGrip]				= action_color_00_default;
				colors[ImGuiCol_ResizeGripHovered]		= action_color_00_hover;
				colors[ImGuiCol_ResizeGripActive]		= action_color_00_active;
				colors[ImGuiCol_Tab]					= LERP_MAIN_COLOR_DARK(0.4f);
				colors[ImGuiCol_TabHovered]				= LERP_MAIN_COLOR_DARK(0.5f);
				colors[ImGuiCol_TabActive]				= LERP_MAIN_COLOR_DARK(0.6f);
				colors[ImGuiCol_TabUnfocused]			= LERP_MAIN_COLOR_DARK(0.5f);
				colors[ImGuiCol_TabUnfocusedActive]		= LERP_MAIN_COLOR_DARK(0.6f);
				colors[ImGuiCol_DockingPreview]			= action_color_00_active;
				colors[ImGuiCol_DockingEmptyBg]			= LERP_GRAY(0.2f);
				colors[ImGuiCol_PlotLines]				= ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered]		= ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
				colors[ImGuiCol_PlotHistogram]			= LERP_MAIN_COLOR_DARK(.75f);
				colors[ImGuiCol_PlotHistogramHovered]	= action_color_00_active;
				colors[ImGuiCol_TableHeaderBg]			= ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
				colors[ImGuiCol_TableBorderStrong]		= ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableBorderLight]		= ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableRowBg]				= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_TableRowBgAlt]			= ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
				colors[ImGuiCol_TextSelectedBg]			= LERP_MAIN_COLOR_DARK(.4f);
				colors[ImGuiCol_DragDropTarget]			= LERP_MAIN_COLOR_DARK(.6f);
				colors[ImGuiCol_NavHighlight]			= ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight]	= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg]		= ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg]		= ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

			} break;

			case PFF::UI::theme_selection::light: {

				action_color_00_default = LERP_MAIN_COLOR_LIGHT(0.7f);
				action_color_00_hover = LERP_MAIN_COLOR_LIGHT(0.85f);
				action_color_00_active = LERP_MAIN_COLOR_LIGHT(1.f);

				action_color_gray_default = LERP_GRAY(0.2f);
				action_color_gray_hover = LERP_GRAY(0.27f);
				action_color_gray_active = LERP_GRAY(0.35f);

				highlited_window_bg = LERP_GRAY(0.8f);
				main_titlebar_color = LERP_MAIN_COLOR_LIGHT(.5f);

				colors[ImGuiCol_Text]					= LERP_GRAY(1.0f);
				colors[ImGuiCol_TextDisabled]			= LERP_GRAY(.7f);
				colors[ImGuiCol_WindowBg]				= LERP_GRAY(.25f);
				colors[ImGuiCol_ChildBg]				= LERP_GRAY(.25f);
				colors[ImGuiCol_PopupBg]				= LERP_GRAY(.25f);
				colors[ImGuiCol_Border]					= LERP_GRAY_A(0.2f, .50f);
				colors[ImGuiCol_BorderShadow]			= LERP_GRAY(.12f);
				colors[ImGuiCol_FrameBg]				= LERP_GRAY_A(.75f, .75f);
				colors[ImGuiCol_FrameBgHovered]			= LERP_GRAY_A(.70f, .75f);
				colors[ImGuiCol_FrameBgActive]			= LERP_GRAY_A(.65f, .75f);
				colors[ImGuiCol_TitleBg]				= action_color_00_default;
				colors[ImGuiCol_TitleBgActive]			= action_color_00_active;
				colors[ImGuiCol_TitleBgCollapsed]		= action_color_00_default;
				colors[ImGuiCol_MenuBarBg]				= LERP_GRAY(.58f);
				colors[ImGuiCol_ScrollbarBg]			= LERP_GRAY(.75f);
				colors[ImGuiCol_ScrollbarGrab]			= action_color_00_default;
				colors[ImGuiCol_ScrollbarGrabHovered]	= action_color_00_hover;
				colors[ImGuiCol_ScrollbarGrabActive]	= action_color_00_active;
				colors[ImGuiCol_CheckMark]				= action_color_00_active;
				colors[ImGuiCol_SliderGrab]				= action_color_00_default;
				colors[ImGuiCol_SliderGrabActive]		= action_color_00_active;
				colors[ImGuiCol_Button]					= action_color_00_default;
				colors[ImGuiCol_ButtonHovered]			= action_color_00_hover;
				colors[ImGuiCol_ButtonActive]			= action_color_00_active;
				colors[ImGuiCol_Header]					= LERP_GRAY(.75f);
				colors[ImGuiCol_HeaderHovered]			= LERP_GRAY(.7f);
				colors[ImGuiCol_HeaderActive]			= LERP_GRAY(.65f);
				colors[ImGuiCol_Separator]				= LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorHovered]		= LERP_GRAY(.45f);
				colors[ImGuiCol_SeparatorActive]		= LERP_GRAY(.45f);
				colors[ImGuiCol_ResizeGrip]				= action_color_00_default;
				colors[ImGuiCol_ResizeGripHovered]		= action_color_00_hover;
				colors[ImGuiCol_ResizeGripActive]		= action_color_00_active;
				colors[ImGuiCol_Tab]					= action_color_00_default;
				colors[ImGuiCol_TabHovered]				= action_color_00_hover;
				colors[ImGuiCol_TabActive]				= action_color_00_active;
				colors[ImGuiCol_TabUnfocused]			= LERP_MAIN_COLOR_LIGHT(0.5f);
				colors[ImGuiCol_TabUnfocusedActive]		= LERP_MAIN_COLOR_LIGHT(0.6f);
				colors[ImGuiCol_DockingPreview]			= action_color_00_active;
				colors[ImGuiCol_DockingEmptyBg]			= LERP_GRAY(.2f);
				colors[ImGuiCol_PlotLines]				= ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered]		= ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
				colors[ImGuiCol_PlotHistogram]			= LERP_MAIN_COLOR_LIGHT(.75f);
				colors[ImGuiCol_PlotHistogramHovered]	= action_color_00_active;
				colors[ImGuiCol_TableHeaderBg]			= ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
				colors[ImGuiCol_TableBorderStrong]		= ImVec4(0.31f, 0.31f, 0.35f, 1.00f);	// Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableBorderLight]		= ImVec4(0.23f, 0.23f, 0.25f, 1.00f);		// Prefer using Alpha=1.0 here
				colors[ImGuiCol_TableRowBg]				= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_TableRowBgAlt]			= ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
				colors[ImGuiCol_TextSelectedBg]			= LERP_MAIN_COLOR_LIGHT(.4f);
				colors[ImGuiCol_DragDropTarget]			= ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
				colors[ImGuiCol_NavHighlight]			= ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight]	= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg]		= ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg]		= ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

			} break;

			default:
				break;
		}

	}

}
