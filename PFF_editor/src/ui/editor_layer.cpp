
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "util/pffpch.h"
#include "editor_layer.h"
#include "application.h"


namespace PFF {


	editor_layer::~editor_layer() {
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

	void editor_layer::general_debugger() {

		if (!m_show_general_debugger)
			return;
		
		ImGuiWindowFlags window_flags{};
		static bool show_viewport = true;
		ImGui::Begin("viewport test", &show_viewport, window_flags);
		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {
			if (ImGui::BeginTabItem("Inputs")) {

				for (input_action& current_action : application::get().get_world_layer()->get_current_player_controller()->m_input_mapping) {						// get input_action
					
					if (ImGui::CollapsingHeader(current_action.name.c_str())) {

						ImGui::Text("%s", current_action.description.c_str());
						switch (current_action.value) {
						
							case input_value::IV_bool: {
								display_value_bool(current_action.data.boolean);
							} break;
						
							case input_value::IV_float: {
								display_value_num(current_action.data.axis_1d);

							} break;
						
							case input_value::IV_vec2: {
								display_value_vec2(current_action.data.axis_2d);

							} break;
							default:
								break;
						}
					}
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Test")) {
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Sizes")) {
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	void editor_layer::show_current_input() {
	}

	void editor_layer::display_value_bool(bool value) {
	}

	void editor_layer::display_value_num(f32 value) {
	}

	void editor_layer::display_value_vec2(glm::vec2 value) {
	}

	void editor_layer::set_next_window_pos(int16 location) {
	}

	void editor_layer::progressbar_with_text(f32 percent, const char* text, f32 min_size_x, f32 min_size_y) {
	}

	void editor_layer::progressbar(f32 percent, f32 min_size_x, f32 min_size_y) {
	}

}