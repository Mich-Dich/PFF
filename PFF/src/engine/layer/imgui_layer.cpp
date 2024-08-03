
#include "util/pffpch.h"

#include <imgui.h>

//#include "engine/render/image.h"
#include "util/ui/window_images.embed"

#include "layer.h"
#include "application.h"
#include "engine/render/renderer.h"
#include "util/ui/pannel_collection.h"

#include "imgui_layer.h"


#define PFF_UI_ACTIVE_THEME					PFF::UI::THEME::current_theme

#define LERP_GRAY_A(value, alpha)			{value, value, value, alpha }
#define LERP_GRAY(value)					{value, value, value, 1.f }
#define IMCOLOR_GRAY(value)					ImColor{value, value, value, 255 }
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

		serializer::yaml(config::get_filepath_from_configtype(config::file::ui), "performance_display", option)
			.entry(KEY_VALUE(m_show_FPS_window))
			.entry(KEY_VALUE(m_show_renderer_metrik));
	}

	static void serialize(serializer::option option) {

		serializer::yaml(config::get_filepath_from_configtype(config::file::ui), "theme", option)
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

		std::string buffer = option == serializer::option::load_from_file ? "load_from_file" : "save_to_file";
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

		action_color_gray_default = LERP_GRAY(0.2f);
		action_color_gray_hover = LERP_GRAY(0.27f);
		action_color_gray_active = LERP_GRAY(0.35f);

		//{
		//	u32 w, h;
		//	void* data = image::decode(g_WindowMaximizeIcon, sizeof(g_WindowMaximizeIcon), w, h);
		//	image test_image = image(data, w, h);

		//	//test_image.reset();
		//}
	}

	imgui_layer::~imgui_layer() { 
		
		CORE_LOG_SHUTDOWN();
	}


	void imgui_layer::on_attach() { 

		LOG(Trace, "attach imgui layer"); 
	
		IMGUI_CHECKVERSION();
		m_context = ImGui::CreateContext();
		GET_RENDERER.imgui_init();
		
		serialize(serializer::option::load_from_file);

		std::filesystem::path base_path = std::filesystem::path("..") / "PFF" / "assets" / "fonts" / "Open_Sans" / "static";

		// Load fonts
		auto io = ImGui::GetIO();
		io.FontAllowUserScaling = true;
		m_fonts["default"] =		io.Fonts->AddFontFromFileTTF((base_path / "OpenSans-Regular.ttf").string().c_str(), m_font_size);
		m_fonts["bold"] =			io.Fonts->AddFontFromFileTTF((base_path / "OpenSans-Bold.ttf").string().c_str(), m_font_size);
		m_fonts["italic"] =			io.Fonts->AddFontFromFileTTF((base_path / "OpenSans-Italic.ttf").string().c_str(), m_font_size);

		m_fonts["regular_big"] =	io.Fonts->AddFontFromFileTTF((base_path / "OpenSans-Regular.ttf").string().c_str(), m_big_font_size);
		m_fonts["bold_big"] =		io.Fonts->AddFontFromFileTTF((base_path / "OpenSans-Bold.ttf").string().c_str(), m_big_font_size);
		m_fonts["italic_big"] =		io.Fonts->AddFontFromFileTTF((base_path / "OpenSans-Italic.ttf").string().c_str(), m_big_font_size);

		m_fonts["header_0"] =		io.Fonts->AddFontFromFileTTF((base_path / "OpenSans-Regular.ttf").string().c_str(), m_font_size_header_2);
		m_fonts["header_1"] =		io.Fonts->AddFontFromFileTTF((base_path / "OpenSans-Regular.ttf").string().c_str(), m_font_size_header_1);
		m_fonts["header_2"] =		io.Fonts->AddFontFromFileTTF((base_path / "OpenSans-Regular.ttf").string().c_str(), m_font_size_header_0);

		m_fonts["giant"] =			io.Fonts->AddFontFromFileTTF((base_path / "OpenSans-Bold.ttf").string().c_str(), 30.f);
		io.FontDefault = m_fonts["default"];

		GET_RENDERER.imgui_create_fonts();
		load_UI_data();
	}


	void imgui_layer::on_detach() { 
		
		LOG(Trace, "detach imgui layer");

		serialize(serializer::option::save_to_file);
		GET_RENDERER.imgui_shutdown();
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
	}


	void imgui_layer::show_renderer_metrik() {

		if (!m_show_renderer_metrik)
			return;

		static UI::window_pos location = UI::window_pos::top_left;
		ImGuiWindowFlags window_flags = (
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav);
		if (location != UI::window_pos::custom)
			window_flags |= ImGuiWindowFlags_NoMove;

		static char formatted_text[32];

		UI::set_next_window_pos_in_window(location);
		ImGui::SetNextWindowBgAlpha(0.8f); // Transparent background
		if (ImGui::Begin("Renderer Metrik##PFF_Engine", &m_show_FPS_window, window_flags)) {

			if (UI::begin_table("Performance Display", true, ImVec2(200.0f, 0))) {

				auto* metrik = PFF::render::vulkan::vk_renderer::get().get_renderer_metrik_pointer();
				UI::table_row_text("mesh draws", "%d", metrik->mesh_draw);
				UI::table_row_text("draw calls", "%d", metrik->draw_calls);
				UI::table_row_text("vertecies", "%d", metrik->vertecies);

				UI::table_row_text("work time", "%d", metrik->work_time);
				UI::table_row_text("sleep time", "%d", metrik->sleep_time);

				UI::end_table();
			}

			UI::next_window_position_selector_popup(location, m_show_renderer_metrik);
		}
		ImGui::End();
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

		// Check if the time since the last update exceeds the update interval
		if (current_time - last_update_time >= update_interval) {

			frame_times[array_pointer % array_size] = (f32)m_current_fps;
			array_pointer = (array_pointer + 1) % array_size;
			last_update_time = current_time;
		}

		static UI::window_pos location = UI::window_pos::top_right;
		ImGuiWindowFlags window_flags = (
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav);
		if (location != UI::window_pos::custom)
			window_flags |= ImGuiWindowFlags_NoMove;

		UI::set_next_window_pos_in_window(location);
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

			if (UI::begin_table("Performance Display", true, ImVec2(200.0f, 0))) {

				if (m_limit_fps)
					UI::table_row("FPS", util::format_string(std::setw(4), m_current_fps, " / ", std::setw(4), m_target_fps));
				else
					UI::table_row("FPS", util::format_string(std::setw(4), m_current_fps));

				snprintf(formatted_text, sizeof(formatted_text), "%5.2f ms", m_work_time);
				UI::table_row_progressbar("work time:", formatted_text, work_percent);

				snprintf(formatted_text, sizeof(formatted_text), "%5.2f ms", m_sleep_time);
				UI::table_row_progressbar("sleep time:", formatted_text, sleep_percent);


				UI::end_table();
			}
			
			UI::shift_cursor_pos(0, 10);

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, vector_multi(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg), ImVec4{ 1, 1, 1, 0 }));
			ImGui::PlotLines("##frame_times", frame_times, IM_ARRAYSIZE(frame_times), (array_pointer % array_size), (const char*)0, 0.0f, FLT_MAX, ImVec2(0, 70));
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			// Plot Lines
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImVec2 plot_size = ImGui::GetItemRectSize();
			ImVec2 plot_pos = ImGui::GetItemRectMin();
			ImVec2 plot_max_pos = { plot_pos.x + plot_size.x , plot_pos.y + plot_size.y };
			static const char* fps_text = "  0 10 20 30 40 50 60 70 80 90100110120130140150160170180190200210220230240250260270280290300310320330340350360370380390400410420430450460470480490500510520530540550560570580590600610620630640650660670680690700710720730740750760770780790800810820830840850860870890900";
			static const u32 offset = static_cast<u32>(ImGui::GetTextLineHeight() / 2);
			static const u32 interval_thin = 10;
			static const u64 interval_thick = 50;

			f32 v_max = 0;
			for (int i = 0; i < array_size; i++) {
				const f32 v = frame_times[(array_pointer + i) % array_size];
				if (v != v) // Ignore NaN values
					continue;

				v_max = math::max(v_max, v);
			}
			const f32 max_value = v_max;

			u32 num_of_displayed_texts = 0;
			u32 buffer = static_cast<u32>(max_value / 7);

			// FREE SECTION FOR TEXT
			//		begin:	55 from right
			//		end:	20 from right

#if 0
			static const u32 NUM_OF_VERTICAL_LINES = 10;
			for (u32 x = 0; x < NUM_OF_VERTICAL_LINES; x++) {

				ImU32 color = (x == NUM_OF_VERTICAL_LINES-1) ? IM_COL32(action_color_00_active.x * 255, action_color_00_active.y * 255, action_color_00_active.z * 255, 255) : IM_COL32(200, 200, 200, 180);
				
				f32 multi = (f32) ((array_pointer + (x * NUM_OF_VERTICAL_LINES)) % array_size) / array_size;
				f32 line_x_position = plot_max_pos.x - (plot_size.x * multi);

				if(line_x_position <= plot_max_pos.x - 50 || line_x_position >= plot_max_pos.x - 20)
					draw_list->AddLine(ImVec2(line_x_position, plot_pos.y - 5), ImVec2(line_x_position, plot_max_pos.y), color);
			}
#endif

			for (u64 i = 0; i <= max_value; i += interval_thin) {

				const char* text = fps_text + ((i / static_cast<u64>(interval_thin)) * 3);
				float y = plot_max_pos.y - (i / max_value * plot_size.y);
				ImU32 color = (i % interval_thick == 0) ? IM_COL32(action_color_00_active.x * 255, action_color_00_active.y * 255, action_color_00_active.z * 255, 255) : IM_COL32(200, 200, 200, 180);

				if (i > (buffer) * num_of_displayed_texts) {

					draw_list->AddLine(ImVec2(plot_pos.x, y), ImVec2(plot_max_pos.x - 55, y), color);
					draw_list->AddText(ImVec2(plot_max_pos.x - 50, y - offset), IM_COL32(200, 200, 200, 180), text, text + 3);
					draw_list->AddLine(ImVec2(plot_max_pos.x - 20, y), ImVec2(plot_max_pos.x, y), color);
					num_of_displayed_texts++;
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

				UI::next_window_position_selector(location, m_show_FPS_window);

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

				ImColor default_gray = IMCOLOR_GRAY(30);
				ImColor default_gray_1 = IMCOLOR_GRAY(35);

				action_color_00_faded = LERP_MAIN_COLOR_DARK(0.5f);
				action_color_00_weak = LERP_MAIN_COLOR_DARK(0.6f);
				action_color_00_default = LERP_MAIN_COLOR_DARK(0.7f);
				action_color_00_hover = LERP_MAIN_COLOR_DARK(0.85f);
				action_color_00_active = LERP_MAIN_COLOR_DARK(0.92f);

				action_color_gray_default = LERP_GRAY(0.15f);
				action_color_gray_hover = LERP_GRAY(0.2f);
				action_color_gray_active = LERP_GRAY(0.25f);

				//highlited_window_bg = LERP_GRAY(0.17f);
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
				colors[ImGuiCol_TitleBg]				= default_gray;
				colors[ImGuiCol_TitleBgActive]			= default_gray;
				colors[ImGuiCol_TitleBgCollapsed]		= default_gray;
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
				colors[ImGuiCol_DragDropTarget]			= ImColor(10, 220, 10, 230);
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
