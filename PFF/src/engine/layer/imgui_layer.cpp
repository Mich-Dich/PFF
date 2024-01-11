
#include "util/pffpch.h"

#include "layer.h"
#include "imgui_layer.h"
#include "application.h"
#include "engine/platform/pff_window.h"
#include "engine/render/renderer.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <vulkan/vulkan.h>

namespace PFF {

	static bool showdemo_window = true;

	imgui_layer::imgui_layer(std::shared_ptr<renderer> renderer)
		: layer("ImGuiLayer"), m_renderer(renderer) {

		LOG(Info, "Init imgui layer");
	}

	imgui_layer::~imgui_layer() {

		LOG(Info, "shutdown imgui layer");
	}

	void imgui_layer::on_attach() {

		LOG(Info, "attach imgui layer");

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		ImGui::StyleColorsDark();
		
		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		;

		application& app = application::get();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(app.get_window()->get_window(), true);
		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.Instance = m_renderer->get_device()->get_instance();
		init_info.PhysicalDevice = m_renderer->get_device()->get_physical_device();
		init_info.Device = m_renderer->get_device()->get_device();
		init_info.QueueFamily = m_renderer->get_device()->find_physical_queue_families().graphicsFamily;
		init_info.Queue = m_renderer->get_device()->get_graphics_queue();
		init_info.DescriptorPool = m_renderer->get_imgui_descriptor_pool();
		init_info.Subpass = m_renderer->get_render_system_pipeline_subpass();
		init_info.MinImageCount = m_renderer->get_device()->get_swap_chain_support().capabilities.minImageCount;
		init_info.ImageCount = m_renderer->get_swapchain_image_count();
		//init_info.PipelineCache = g_PipelineCache;
		//init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		//init_info.Allocator = nullptr;
		//init_info.CheckVkResultFn = check_vk_result;
		CORE_ASSERT(ImGui_ImplVulkan_Init(&init_info, m_renderer->get_swapchain_render_pass()), "", "");

		// Use any command queue
		VkCommandBuffer command_buffer = m_renderer->get_device()->begin_single_time_commands();
		ImGui_ImplVulkan_CreateFontsTexture();
		m_renderer->get_device()->end_single_time_commands(command_buffer);

		CORE_ASSERT(vkDeviceWaitIdle(m_renderer->get_device()->get_device()) == VK_SUCCESS, "", "Failed wait idle");
	}

	void imgui_layer::on_detach() {

		LOG(Info, "detach imgui layer");
		
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void imgui_layer::on_update() {
	}

	void imgui_layer::on_event(event& event) {
	}

	void imgui_layer::on_imgui_render() {
		
		ImGui::ShowDemoWindow(&showdemo_window);
	}

	void imgui_layer::begin_frame() {

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void imgui_layer::end_frame(VkCommandBuffer commandbuffer) {

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandbuffer, 0);

	}

}
