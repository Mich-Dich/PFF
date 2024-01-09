
#include "util/pffpch.h"

#include "layer.h"
#include "imgui_layer.h"

#include <vulkan/vulkan.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imconfig.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "imgui_tables.cpp"
#include "imgui_internal.h"
#include "imgui.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "imgui_demo.cpp"
#include "imgui_impl_glfw.cpp"
//#include "imgui_impl_vulkan_but_better.h"

#include "engine/render/vk_device.h"
#include "engine/render/renderer.h"

#include "imgui.h"

namespace PFF {

	imgui_layer::imgui_layer(std::shared_ptr<vk_device> device)
		: layer("ImGuiLayer"), m_device(device){

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark();

		// Use any command queue
		VkCommandBuffer command_buffer = m_device->begin_single_time_commands();
		ImGui_ImplVulkan_CreateFontsTexture();
		m_device->end_single_time_commands(command_buffer);

		CORE_ASSERT(vkDeviceWaitIdle(m_device->get_device()) == VK_SUCCESS, "", "Failed wait idle");
	}

	imgui_layer::~imgui_layer() {

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void imgui_layer::on_attach() {
	}

	void imgui_layer::on_detach() {
	}

	void imgui_layer::on_update() {
	}

	void imgui_layer::on_imgui_render() {

		ImGui::ShowDemoWindow();
	}

	void imgui_layer::begin_frame() {

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void imgui_layer::end_frame(VkCommandBuffer command_buffer) {

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer, 0);
	}

}