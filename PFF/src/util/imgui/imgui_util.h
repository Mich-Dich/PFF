#pragma once


namespace PFF::UI::imgui::util {


	static ImGuiContext* s_context{};

	static f32 s_font_size = 15.f;
	static f32 s_big_font_size = 18.f;
	static std::unordered_map<std::string, ImFont*> s_fonts{};

	enum class window_pos {

		center			= 0,
		custom			= 1,
		top_left		= 2,
		top_right		= 3,
		bottom_left		= 4,
		bottom_right	= 5,
	};

	//ImFont* PFF_API get_font(const std::string& name);

	void PFF_API set_next_window_pos(window_pos location, f32 padding = 10.f);

	void PFF_API set_next_window_pos_in_window(window_pos location, f32 padding = 10.f);

	void PFF_API next_window_position_selector(window_pos& position, bool& show_window);
	
	void PFF_API progressbar_with_text(const char* lable, const char* progress_bar_text, f32 percent, f32 lable_size = 50.f, f32 progressbar_size_x = 50.f, f32 progressbar_size_y = 1.f);

}
