
#include "util/pffpch.h"

#include "layer.h"
#include "imgui_layer.h"

#include "imgui.h"

namespace PFF {

	static bool showdemo_window = true;

	imgui_layer::imgui_layer() 
		: layer("ImGuiLayer") {

		LOG(Info, "Init Imgui layer");
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	imgui_layer::~imgui_layer() {

		LOG(Info, "shutdown Imgui layer");
		ImGui::DestroyContext();
	}

	void imgui_layer::on_attach() {
	}

	void imgui_layer::on_detach() {
	}

	void imgui_layer::on_update() {
	}

	void imgui_layer::on_event(event& event) {
	}

	void imgui_layer::on_imgui_render() {
		
		ImGui::ShowDemoWindow(&showdemo_window);
	}

	void imgui_layer::begin_frame() {
	}

	void imgui_layer::end_frame() {
	}

}
