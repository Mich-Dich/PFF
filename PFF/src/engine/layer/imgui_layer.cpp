
#include "util/pffpch.h"

#include "layer.h"
#include "imgui_layer.h"
#include "application.h"
#include "engine/platform/pff_window.h"
#include "engine/render/renderer.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <vulkan/vulkan.h>

namespace PFF {

#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)

	static bool showdemo_window = true;
	static bool show_fps = true;

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
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		ImGui::StyleColorsDark();
		
		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

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

		// Modify the color of the progress bar
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.05f, 0.5f, 0.05f, 1.0f));

	}

	void imgui_layer::on_detach() {

		LOG(Info, "detach imgui layer");
		
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void imgui_layer::on_update(f32 delta_time) {
	}

	void imgui_layer::on_event(event& event) {


		ImGuiIO& io = ImGui::GetIO();
		if (event.is_in_category(EC_Mouse) && io.WantCaptureMouse) {

			event.handled = true;
			// CORE_LOG(Trace, "MOUSE EVENT")
		}
	}

	void imgui_layer::on_imgui_render() {

		// Demonstrate creating a simple static window with no decoration
		// + a context-menu to choose which corner of the screen to use.
		static int location = 1;
		ImGuiWindowFlags window_flags;
		//ImGuiIO& io = ImGui::GetIO();

		if (show_fps) {

			set_next_window_pos(location, window_flags);
			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
			if (ImGui::Begin("performance_timer", &show_fps, window_flags)) {

				// Get the current style
				ImGuiStyle& style = ImGui::GetStyle();

				// Get the line spacing (vertical padding around text)
				f32 lineSpacing = style.ItemSpacing.y / 2;
				f32 fontSize = ImGui::GetFontSize();
				f32 work_percent = m_work_time / (m_work_time + m_sleep_time);
				f32 sleep_percent = 1 - work_percent;
				char formattedText[32];
				ImVec2 curser_pos;
				ImVec2 textSize;

				ImGui::Text("performance timer");
				ImGui::Separator();
				ImGui::Text("current fps [%d/%d]", m_current_fps, m_target_fps);

				// work_time
				ImGui::Text("work time  ");
				ImGui::SameLine();
				snprintf(formattedText, sizeof(formattedText), " %7.2f ", m_work_time);
				progressbar_with_text(work_percent, formattedText);
				ImGui::SameLine();
				ImGui::Text("ms");

				// sleep time
				ImGui::Text("sleep time ");
				ImGui::SameLine();
				snprintf(formattedText, sizeof(formattedText), " %7.2f ", m_sleep_time);
				progressbar_with_text(sleep_percent, formattedText);
				ImGui::SameLine();
				ImGui::Text("ms");
				

				if (ImGui::BeginPopupContextWindow()) {
					if (ImGui::MenuItem("custom", NULL, location == -1)) location = -1;
					if (ImGui::MenuItem("center", NULL, location == -2)) location = -2;
					if (ImGui::MenuItem("top-left", NULL, location == 0)) location = 0;
					if (ImGui::MenuItem("top-right", NULL, location == 1)) location = 1;
					if (ImGui::MenuItem("bottom-left", NULL, location == 2)) location = 2;
					if (ImGui::MenuItem("bottom-right", NULL, location == 3)) location = 3;
					if (&show_fps && ImGui::MenuItem("close")) show_fps = false;
					ImGui::EndPopup();
				}
			}
			ImGui::End();
		}


		ImGui::ShowDemoWindow(&showdemo_window);
	}

	//f32 m_target_fps, m_current_fps, m_possible_fps, m_work_time;

	void imgui_layer::begin_frame() {

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void imgui_layer::end_frame(VkCommandBuffer commandbuffer) {

		ImGuiIO& io = ImGui::GetIO();
		application& app = application::get();
		io.DisplaySize = ImVec2(static_cast<f32>(app.get_window()->get_width()), static_cast<f32>(app.get_window()->get_height()));

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandbuffer, 0);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {

			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void imgui_layer::set_next_window_pos(int16 location, ImGuiWindowFlags& window_flags) {

		window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		if (location >= 0) {

			const float PAD = 10.0f;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
			ImVec2 work_size = viewport->WorkSize;
			ImVec2 window_pos, window_pos_pivot;
			window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
			window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
			window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
			window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowViewport(viewport->ID);
			window_flags |= ImGuiWindowFlags_NoMove;
		} else if (location == -2) {
			// Center window
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			window_flags |= ImGuiWindowFlags_NoMove;
		}
	}

	void imgui_layer::progressbar_with_text(f32 percent, const char* text) {

		ImVec2 curser_pos;
		ImVec2 textSize;

		curser_pos = ImGui::GetCursorScreenPos();
		ImGui::SetNextItemAllowOverlap();

		textSize = ImGui::CalcTextSize(text);
		ImGui::ProgressBar(percent, textSize, "");
		ImGui::SetCursorScreenPos(curser_pos);
		ImGui::Text("%s", text);
	}

}
