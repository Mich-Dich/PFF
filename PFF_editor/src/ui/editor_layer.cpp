
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>


#include "ui/panels/pannel_collection.h"

#include "util/pffpch.h"
#include "editor_layer.h"
#include "application.h"


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
		main_menu_bar();
		general_debugger();

		ImGui::ShowDemoWindow();
	}

	void editor_layer::main_menu_bar() {

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
			ImGui::EndMainMenuBar();
		}
	}
	void DrawVerticalSeparator(float thickness, ImVec4 color) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImDrawList* draw_list = window->DrawList;

		ImVec2 window_pos = window->DC.CursorPos;
		ImVec2 content_max = ImGui::GetWindowContentRegionMax();

		draw_list->AddLine(
			ImVec2(window_pos.x + content_max.x, window_pos.y),
			ImVec2(window_pos.x + content_max.x, window_pos.y + content_max.y),
			IM_COL32(color.x * 255, color.y * 255, color.z * 255, color.w * 255),
			thickness
		);
	}

	void DrawTwoTextsWithVerticalSeparator(const char* text1, const char* text2) {
		// Draw the first text
		ImGui::Text("%s", text1);

		// Draw a vertical separator
		DrawVerticalSeparator(2.0f, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		// Draw the second text
		ImGui::Text("%s", text2);
	}

	void editor_layer::general_debugger() {

		if (!m_show_general_debugger)
			return;
		
		ImGuiWindowFlags window_flags{};
		static bool show_viewport = true;
		
		if (ImGui::Begin("Editor Debugger", &show_viewport, window_flags)) {			
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
						{
							display_column(action->name, action->data.boolean);
						} break;

						case input_value::_1D:
						{
							display_column(action->name, action->data._1D);
						} break;

						case input_value::_2D:
						{
							display_column(action->name, action->data._2D);
						} break;

						case input_value::_3D:
						{
							display_column(action->name, action->data._3D);
						} break;

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


		/*
		if (ImGui::Begin("Outliner", &show_viewport, window_flags)) {

		}
		ImGui::End();

		if (ImGui::Begin("Details", &show_viewport, window_flags)) {

		}
		ImGui::End();

		if (ImGui::Begin("World Settings", &show_viewport, window_flags)) {

		}
		ImGui::End();
		*/
	}

	void editor_layer::show_current_input() {
	}

	void editor_layer::display_value_bool(bool value) {
	}

	void editor_layer::display_value_num(f32 value) {
	}


	void editor_layer::set_next_window_pos(int16 location) {
	}

	void editor_layer::progressbar_with_text(f32 percent, const char* text, f32 min_size_x, f32 min_size_y) {
	}

	void editor_layer::progressbar(f32 percent, f32 min_size_x, f32 min_size_y) {
	}

}