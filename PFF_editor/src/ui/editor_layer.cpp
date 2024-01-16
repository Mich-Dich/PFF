
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "util/pffpch.h"
#include "editor_layer.h"
#include "application.h"


namespace PFF {



	editor_layer::editor_layer() {
	}

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

		static bool show = true;
		//ImGui::ShowDemoWindow(&show);

		/*
		if (ImGui::BeginMainMenuBar()) {
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
				if (ImGui::MenuItem("Options")) {}
				if (ImGui::MenuItem("Checked", NULL, true)) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Quit", "Alt+F4")) {
					application::get().close_application();
				}
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
		*/
	}

	void editor_layer::set_next_window_pos(int16 location) {
	}

	void editor_layer::progressbar_with_text(f32 percent, const char* text, f32 min_size_x, f32 min_size_y) {
	}

	void editor_layer::progressbar(f32 percent, f32 min_size_x, f32 min_size_y) {
	}

}