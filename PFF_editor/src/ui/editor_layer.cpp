
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>

#include "ui/panels/pannel_collection.h"
#include "util/pffpch.h"
#include "engine/platform/pff_window.h"
#include "engine/color_theme.h"
#include "application.h"
#include "engine/layer/imgui_layer.h"
#include "engine/platform/pff_window.h"

#include "editor_layer.h"


namespace PFF {

	editor_layer::~editor_layer() {

		LOG(Info, "Shutdown");
	}

	void editor_layer::on_attach() {
	}

	void editor_layer::on_detach() {
	}

	void editor_layer::on_update(f32 delta_time) {
	}

	void editor_layer::on_event(event& event) {
	}

	void editor_layer::on_imgui_render() {

		ImGui::SetCurrentContext(m_context);

		window_main_menu_bar();
		// window_main_content();

		window_general_debugger();
		window_outliner();
		window_details();
		window_world_settings();
		window_content_browser_0();
		window_content_browser_1();

		ImGui::ShowDemoWindow();				// DEV-ONLY
	}


	void editor_layer::window_main_menu_bar() {

		const f32 titlebar_height = 60.f;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, titlebar_height));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		// ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		// ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDocking;
		// window_flags |= ImGuiWindowFlags_MenuBar;
		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, PFF_UI_ACTIVE_THEME->WindowBg);
		if (ImGui::Begin("DockSpaceWindow", nullptr, window_flags)) {

			ImGui::PopStyleColor();

			const auto window_sie_buf = application::get().get_window()->get_window_size_state();
			const bool is_maximized = (window_sie_buf == window_size_state::fullscreen);
			float titlebar_vertical_offset = is_maximized ? -100.f : 0.f;
			const ImVec2 window_padding = ImGui::GetCurrentWindow()->WindowPadding;

			ImGui::SetCursorPos(ImVec2(window_padding.x, window_padding.y + titlebar_vertical_offset));
			const ImVec2 titlebar_min = ImGui::GetCursorScreenPos();
			const ImVec2 titlebar_max = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - window_padding.x * 2.f,
											ImGui::GetCursorScreenPos().y + titlebar_height - window_padding.y * 2.f };

			auto* bg_draw_list = ImGui::GetBackgroundDrawList();
			auto* fg_draw_list = ImGui::GetForegroundDrawList();
			// bg_draw_list->AddRectFilled(titlebar_min, titlebar_max, IM_COL32(21, 251, 21, 255));

			// Debug titlebar bounds
			//fg_draw_list->AddRect(titlebar_min, titlebar_max, IM_COL32(222, 43, 43, 255));

			// LOGO
			{

			}


			static f32 move_offset_y;
			static f32 move_offset_x;
			const f32 w = ImGui::GetContentRegionAvail().x;
			const f32 button_width = 30.f;
			const f32 button_area_width = (button_width * 3) + (window_padding.x * 4);

			// tilebar drag area
			ImGui::SetCursorPos(ImVec2(window_padding.x, window_padding.y + titlebar_vertical_offset));

			// debug Drab bounds
			//fg_draw_list->AddRect(ImGui::GetCursorScreenPos(), ImVec2(w - button_area_width, titlebar_height), IM_COL32(222, 43, 43, 255));

#if 1	// FOR DEBUG VIAUL
			ImGui::InvisibleButton("##titlebar_drag_zone", ImVec2(w - button_area_width, titlebar_height - window_padding.y * 2));
#else
			ImGui::Button("##titlebar_drag_zone", ImVec2(w - button_area_width, titlebar_height - window_padding.y * 2));
#endif // 1

			application::get().set_titlebar_hovered(ImGui::IsItemHovered());
			ImGui::SetItemAllowOverlap();

			// ImGui::Spring();
			ImGui::SetCursorPos(ImVec2(w - button_area_width + window_padding.x * 2, window_padding.y));
			if (ImGui::Button("Min", ImVec2(30.f, 30.f)))
				application::get().minimize_window();
			// UI::DrawButtonImage(m_IconMinimize, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), 0.0f, -padY));

			ImGui::SetCursorPos(ImVec2(w - button_area_width + window_padding.x * 3 + button_width, window_padding.y));
			if (ImGui::Button("Max", ImVec2(30.f, 30.f)))
				application::get().maximize_window();
			// UI::DrawButtonImage(m_IconMinimize, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), 0.0f, -padY));

			ImGui::SetCursorPos(ImVec2(w - button_area_width + window_padding.x * 4 + button_width * 2, window_padding.y));
			if (ImGui::Button("Close", ImVec2(30.f, 30.f)))
				application::get().close_application();
			// UI::DrawButtonImage(m_IconMinimize, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), 0.0f, -padY));


			ImGui::SetCursorPos(ImVec2(window_padding.x + 120, window_padding.y));
			if (ImGui::BeginMenuBar()) {

				if (ImGui::BeginMenu("File")) {

					ImGui::MenuItem("menu", NULL, false, false);
					if (ImGui::MenuItem("New")) {}
					if (ImGui::MenuItem("Open", "Ctrl+O")) {}
					if (ImGui::BeginMenu("Open Recent")) {
						ImGui::MenuItem("not implemented yet");
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem("Save", "Ctrl+S")) {}
					if (ImGui::MenuItem("Save As..")) {}

					ImGui::Separator();
					if (ImGui::MenuItem("Options"), NULL, m_show_options)
						m_show_options = true;
					ImGui::Separator();
					if (ImGui::MenuItem("Quit", "Alt+F4"))
						application::get().close_application();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit")) {
					if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
					if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
					ImGui::Separator();
					if (ImGui::MenuItem("Cut", "CTRL+X")) {}
					if (ImGui::MenuItem("Copy", "CTRL+C")) {}
					if (ImGui::MenuItem("Paste", "CTRL+V")) {}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("settings")) {
					if (ImGui::MenuItem("General Settings")) {}
					if (ImGui::MenuItem("Editor Settings", false, false)) {}  // Disabled item
					ImGui::Separator();
					if (ImGui::BeginMenu("Color Theme")) {
						ImGui::MenuItem("Dark/Green (default)");
						ImGui::MenuItem("Dark/Blue");
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::SetCursorPos(ImVec2(window_padding.x + 120, window_padding.y + titlebar_height / 2));
			if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {

				f32 tab_width = 100.f;		// TODO: move into [default_tab_width] variable in config-file
				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Main Viewport")) {
					ImGui::EndTabItem();
				}

				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Mesh Editor")) {
					ImGui::EndTabItem();
				}

				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Material Editor")) {
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

		} else {

			ImGui::PopStyleColor();
		}

		ImGui::End();


		/*
		
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags main_window_flags = 0;
		// main_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		// main_window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		// window_flags |= ImGuiWindowFlags_MenuBar;

		// ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(100, 4));
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 70));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 400, 400));
		if (ImGui::Begin("Main_window_Area", nullptr, 0)) {


		}
		ImGui::End();
		*/



		/*
		static f32 move_offset_y;
		static f32 move_offset_x;
		const f32 w = ImGui::GetContentRegionAvail().x;
		const f32 button_area_width = 94;

		// tilebar drag area
		ImGui::SetCursorPos(ImVec2(window_padding.x, window_padding.y + titlebar_vertical_offset));

		// debug Drab bounds
		//fg_draw_list->AddRect(ImGui::GetCursorScreenPos(), ImVec2(w - button_area_width, titlebar_height), IM_COL32(222, 43, 43, 255));

		ImGui::InvisibleButton("##titlebar_drag_zone", ImVec2(w - button_area_width, titlebar_height));
		application::get().set_titlebar_hovered(ImGui::IsItemHovered());
		*/
		
		//const ImRect menuBarRect = { ImGui::GetCursorPos(), { ImGui::GetContentRegionAvail().x + ImGui::GetCursorScreenPos().x, ImGui::GetFrameHeightWithSpacing() } };

		/*
		ImGui::Spring();
		UI::ShiftCursorY(8.0f);
		{
			const int iconWidth = m_IconMinimize->GetWidth();
			const int iconHeight = m_IconMinimize->GetHeight();
			const float padY = (buttonHeight - (float)iconHeight) / 2.0f;
			if (ImGui::InvisibleButton("Minimize", ImVec2(buttonWidth, buttonHeight))) {
				// TODO: move this stuff to a better place, like Window class
				if (m_WindowHandle) {
					Application::Get().QueueEvent([windowHandle = m_WindowHandle]() { glfwIconifyWindow(windowHandle); });
				}
			}

			UI::DrawButtonImage(m_IconMinimize, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), 0.0f, -padY));
		}
		*/

		/*
		*/

	}

	void editor_layer::window_general_debugger() {

		if (!m_show_general_debugger)
			return;
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("Editor Debugger", &m_show_general_debugger, window_flags)) {
			if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {

				f32 tab_width = 60.f;		// TODO: move into [default_tab_width] variable in config-file
				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Inputs")) {

					for (input_action* action : *application::get().get_world_layer()->get_current_player_controller()->get_input_mapping()) {						// get input_action

						switch (action->value) {
						case input_value::_bool:
							display_column(action->name, action->data.boolean, ImGuiInputTextFlags_ReadOnly);
							break;

						case input_value::_1D:
							display_column(action->name, action->data._1D, ImGuiInputTextFlags_ReadOnly);
							break;

						case input_value::_2D:
							display_column(action->name, action->data._2D, ImGuiInputTextFlags_ReadOnly);
							break;

						case input_value::_3D:
							display_column(action->name, action->data._3D, ImGuiInputTextFlags_ReadOnly);
							break;

						default:
							break;
						}
					}

					ImGui::EndTabItem();
				}

				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Test")) {

					// camera pos and dir
					if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {

						display_column("Position", glm::vec3(), 0);
						display_column("Direction", glm::vec2(), 0);
					}

					ImGui::EndTabItem();
				}

				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Sizes")) {
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}

	void editor_layer::window_main_content() {

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 100), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 100, 350));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin("Viewport", nullptr, window_flags)) {
		
		}
		ImGui::End();
	}

	void editor_layer::window_outliner() {

		if (!m_show_outliner)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("Outliner", &m_show_outliner, window_flags)) {}

		ImGui::End();
	}

	void editor_layer::window_details() {

		if (!m_show_details)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("Details", &m_show_details, window_flags)) {}

		ImGui::End();
	}

	void editor_layer::window_world_settings() {

		if (!m_show_world_settings)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("World Settings", &m_show_world_settings, window_flags)) {}

		ImGui::End();
	}

	void editor_layer::window_content_browser_0() {

		if (!m_show_content_browser_0)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("Content Browser", &m_show_content_browser_0, window_flags)) {}

		ImGui::End();
	}

	void editor_layer::window_content_browser_1() {

		if (!m_show_content_browser_1)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("Content Browser 2", &m_show_content_browser_1, window_flags)) {}

		ImGui::End();
	}



	void editor_layer::set_next_window_pos(int16 location) {
	}

	void editor_layer::progressbar_with_text(f32 percent, const char* text, f32 min_size_x, f32 min_size_y) {
	}

	void editor_layer::progressbar(f32 percent, f32 min_size_x, f32 min_size_y) {
	}

}


//	ImGuiWindowFlags
//		ImGuiWindowFlags_None 								
//		ImGuiWindowFlags_NoTitleBar 					Disable title-bar
//		ImGuiWindowFlags_NoResize 						Disable user resizing with the lower-right grip
//		ImGuiWindowFlags_NoMove 						Disable user moving the window
//		ImGuiWindowFlags_NoScrollbar 					Disable scrollbars (window can still scroll with mouse or programmatically)
//		ImGuiWindowFlags_NoScrollWithMouse 				Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
//		ImGuiWindowFlags_NoCollapse 					Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
//		ImGuiWindowFlags_AlwaysAutoResize 				Resize every window to its content every frame
//		ImGuiWindowFlags_NoBackground 					Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
//		ImGuiWindowFlags_NoSavedSettings 				Never load/save settings in .ini file
//		ImGuiWindowFlags_NoMouseInputs 					Disable catching mouse, hovering test with pass through.
//		ImGuiWindowFlags_MenuBar 						Has a menu-bar
//		ImGuiWindowFlags_HorizontalScrollbar 			Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
//		ImGuiWindowFlags_NoFocusOnAppearing 			Disable taking focus when transitioning from hidden to visible state
//		ImGuiWindowFlags_NoBringToFrontOnFocus 			Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
//		ImGuiWindowFlags_AlwaysVerticalScrollbar 		Always show vertical scrollbar (even if ContentSize.y < Size.y)
//		ImGuiWindowFlags_AlwaysHorizontalScrollbar 		Always show horizontal scrollbar (even if ContentSize.x < Size.x)
//		ImGuiWindowFlags_NoNavInputs 					No gamepad/keyboard navigation within the window
//		ImGuiWindowFlags_NoNavFocus 					No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
//		ImGuiWindowFlags_UnsavedDocument 				Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
//		ImGuiWindowFlags_NoDocking 						Disable docking of this window
//
//		ImGuiWindowFlags_NoNav 							ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus,
//		ImGuiWindowFlags_NoDecoration 					ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse,
//		ImGuiWindowFlags_NoInputs 						ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus,
//	