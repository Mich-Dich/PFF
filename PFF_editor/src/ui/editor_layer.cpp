
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>

#include "ui/panels/pannel_collection.h"
#include "util/pffpch.h"
#include "application.h"

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
		ImGui_window_main_menu_bar();
		ImGui_window_general_debugger();
		ImGui_window_Outliner();
		ImGui_window_Details();
		ImGui_window_World_Settings();
		ImGui_window_ContentBrowser_0();
		ImGui_window_ContentBrowser_1();

		ImGui::ShowDemoWindow();				// DEV-ONLY
	}

	void editor_layer::ImGui_window_main_menu_bar() {

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {

				ImGui::MenuItem("menu", NULL, false, false);
				if (ImGui::MenuItem("New"))
					{}
				if (ImGui::MenuItem("Open", "Ctrl+O"))
					{}
				if (ImGui::BeginMenu("Open Recent")) {
					ImGui::MenuItem("not implemented yet");
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Save", "Ctrl+S"))
					{}
				if (ImGui::MenuItem("Save As.."))
					{}

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
			ImGui::EndMainMenuBar();
		}
	}

	void editor_layer::ImGui_window_general_debugger() {

		if (!m_show_general_debugger)
			return;
		
		ImGuiWindowFlags window_flags{};		
		if (ImGui::Begin("Editor Debugger", &m_show_general_debugger, window_flags)) {
			if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {

				f32 tab_width = 60.f;		// TODO: move into [default_tab_width] variable in config-file
				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Inputs")) {
#if 0

					ImGui::Columns(2, nullptr, true);

					for (input_action* action : *application::get().get_world_layer()->get_current_player_controller()->get_input_mapping()) {						// get input_action

						ImGui::Text("%s", action->name.c_str());
					}

					ImGui::NextColumn();

					for (input_action* action : *application::get().get_world_layer()->get_current_player_controller()->get_input_mapping()) {						// get input_action

						ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
						switch (action->value) {

						case input_value::_bool:
						{

							ImGui::Text("%s", util::bool_to_str(action->data.boolean));
						} break;

						case input_value::_1D:
						{

							ImGui::DragFloat("##X", &action->data._1D, 0.1f, 0.0f, 0.0f, "%.2f");
						} break;

						case input_value::_2D:
						{

							ImGui::InputFloat2("", &action->data._2D[0], "%.2f");
						} break;

						case input_value::_3D:
						{

							ImGui::InputFloat3("", &action->data._3D[0], "%.2f");
						} break;

						default:
							break;
						}

					}

					ImGui::Columns(1);
#else

					for (input_action* action : *application::get().get_world_layer()->get_current_player_controller()->get_input_mapping()) {						// get input_action

						switch (action->value) {
						case input_value::_bool:
							display_column(action->name, action->data.boolean);
							break;

						case input_value::_1D:
							display_column(action->name, action->data._1D);
							break;

						case input_value::_2D:
							display_column(action->name, action->data._2D);
							break;

						case input_value::_3D:
							display_column(action->name, action->data._3D);
							break;

						default:
							break;
						}
					}
#endif // 0

					ImGui::EndTabItem();
				}

				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Test")) {
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

	void editor_layer::ImGui_window_Outliner() {

		if (!m_show_Outliner)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("Outliner", &m_show_Outliner, window_flags)) {}

		ImGui::End();
	}

	void editor_layer::ImGui_window_Details() {

		if (!m_show_Details)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("Details", &m_show_Details, window_flags)) {}

		ImGui::End();
	}

	void editor_layer::ImGui_window_World_Settings() {

		if (!m_show_World_Settings)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("World Settings", &m_show_World_Settings, window_flags)) {}

		ImGui::End();
	}

	void editor_layer::ImGui_window_ContentBrowser_0() {

		if (!m_show_ContentBrowser_0)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("Content Browser", &m_show_ContentBrowser_0, window_flags)) {}

		ImGui::End();
	}

	void editor_layer::ImGui_window_ContentBrowser_1() {

		if (!m_show_ContentBrowser_1)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("Content Browser 2", &m_show_ContentBrowser_1, window_flags)) {}

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