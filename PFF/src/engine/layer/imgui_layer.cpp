
#include "util/pffpch.h"

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "layer.h"
#include "application.h"
#include "engine/platform/pff_window.h"
#include "engine/render/renderer.h"
#include "engine/color_theme.h"

#include "imgui_layer.h"

namespace PFF {

	template<typename key_type, typename value_type>
	bool contains(const std::unordered_map<key_type, value_type>& map, const key_type& key) {

		return map.find(key) != map.end();
	}

#define LOAD_IMGUI_COLOR_SETTING(key, value)			{ImVec4 buffer_vec = value;																\
														config::load(config::file::editor, "color_scheme", #key, buffer_vec);					\
														ImGui::PushStyleColor(ImGuiCol_##key, buffer_vec);}

#define SAVE_IMGUI_COLOR_SETTING(key, value)			{ImVec4 buffer_vec = value;																\
														config::save(config::file::editor, "color_scheme", #key, buffer_vec);					\
														ImGui::PushStyleColor(ImGuiCol_##key, buffer_vec);}

#ifdef PFF_DEBUG
	#define	IMGUI_COLOR_CONFIG(key, value)				SAVE_IMGUI_COLOR_SETTING(key, value)
#else
	#define	IMGUI_COLOR_CONFIG(key, value)				LOAD_IMGUI_COLOR_SETTING(key, value)
#endif // PFF_DEBUG


#define LOAD_IMGUI_STYLE_SETTING(type, key, value)		{type buffer_vec = value;																\
														config::load(config::file::editor, "color_scheme", #key, buffer_vec);					\
														ImGui::PushStyleVar(ImGuiStyleVar_##key, buffer_vec);}

#define SAVE_IMGUI_STYLE_SETTING(type, key, value)		{type buffer_vec = value;																\
														config::save(config::file::editor, "color_scheme", #key, buffer_vec);					\
														ImGui::PushStyleVar(ImGuiStyleVar_##key, buffer_vec);}

#ifdef PFF_DEBUG
#define	IMGUI_STYLE_CONFIG(type, key, value)			SAVE_IMGUI_STYLE_SETTING(type, key, value)
#else
#define	IMGUI_STYLE_CONFIG(type, key, value)			LOAD_IMGUI_STYLE_SETTING(type, key, value)
#endif // PFF_DEBUG


	static bool showdemo_window = true;
	static bool show_fps = true;

	imgui_layer::imgui_layer(std::shared_ptr<renderer> renderer)
		: layer("ImGuiLayer"), m_renderer(renderer) {

		PFF_PROFILE_FUNCTION();

		LOG(Info, "Init imgui layer");
	}

	imgui_layer::~imgui_layer() {

		PFF_PROFILE_FUNCTION();

		m_renderer.reset();
		CORE_LOG(Info, "Shutdown");
	}

	void imgui_layer::on_attach() {

		PFF_PROFILE_FUNCTION();

		LOG(Info, "attach imgui layer");

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		m_context = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		io.IniFilename = "./config/imgui.ini";
		ImGui::SetCurrentContext(m_context);

		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		application& app = application::get();
		config::load(config::file::editor, "UI", "font_size", m_font_size);

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(app.get_window()->get_window(), true);
		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.Instance = m_renderer->get_device()->get_instance();
		init_info.PhysicalDevice = m_renderer->get_device()->get_physical_device();
		init_info.Device = m_renderer->get_device()->get_device();
		init_info.QueueFamily = m_renderer->get_device()->find_physical_queue_families().graphicsFamily;
		init_info.Queue = m_renderer->get_device()->get_graphics_queue();
		init_info.DescriptorPool = m_renderer->get_global_descriptor_pool();
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
		
		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		m_fonts["default"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Regular.ttf", m_font_size);;
		m_fonts["bold"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Bold.ttf", m_font_size);
		m_fonts["italic"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Italic.ttf", m_font_size);

		m_fonts["regular_big"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Regular.ttf", m_big_font_size);
		m_fonts["bold_big"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Bold.ttf", m_big_font_size);
		m_fonts["italic_big"] = io.Fonts->AddFontFromFileTTF("../PFF/assets/fonts/Open_Sans/static/OpenSans-Italic.ttf", m_big_font_size);
		io.FontDefault = m_fonts["default"];

		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
		m_renderer->get_device()->end_single_time_commands(command_buffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();


		CORE_ASSERT(vkDeviceWaitIdle(m_renderer->get_device()->get_device()) == VK_SUCCESS, "", "Failed wait idle");

		// Modify the color of the progress bar
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.05f, 0.5f, 0.05f, 1.0f));

		IMGUI_COLOR_CONFIG(Header, PFF_UI_ACTIVE_THEME->color_00_default);
		IMGUI_COLOR_CONFIG(HeaderHovered, PFF_UI_ACTIVE_THEME->color_00_hover);
		IMGUI_COLOR_CONFIG(HeaderActive, PFF_UI_ACTIVE_THEME->color_00_active);
		
		IMGUI_COLOR_CONFIG(TabUnfocused, PFF_UI_ACTIVE_THEME->color_01_default);
		IMGUI_COLOR_CONFIG(TabUnfocusedActive, PFF_UI_ACTIVE_THEME->color_01_active);
		
		IMGUI_COLOR_CONFIG(Button, PFF_UI_ACTIVE_THEME->color_01_default);
		IMGUI_COLOR_CONFIG(ButtonHovered, PFF_UI_ACTIVE_THEME->color_01_hover);
		IMGUI_COLOR_CONFIG(ButtonActive, PFF_UI_ACTIVE_THEME->color_01_active);
		
		IMGUI_COLOR_CONFIG(Tab, PFF_UI_ACTIVE_THEME->color_02_default);
		IMGUI_COLOR_CONFIG(TabHovered, PFF_UI_ACTIVE_THEME->color_02_hover);
		IMGUI_COLOR_CONFIG(TabActive, PFF_UI_ACTIVE_THEME->color_02_active);
		
		IMGUI_COLOR_CONFIG(FrameBg, ImVec4(0.f, 0.f, 0.f, 1.0f));
		IMGUI_COLOR_CONFIG(FrameBgHovered, ImVec4(0.1f, 0.4f, 0.1f, 1.0f));
		IMGUI_COLOR_CONFIG(FrameBgActive, ImVec4(0.1f, 0.3f, 0.1f, 1.0f));
		
		IMGUI_COLOR_CONFIG(ResizeGrip, PFF_UI_ACTIVE_THEME->color_01_default);
		IMGUI_COLOR_CONFIG(ResizeGripHovered, PFF_UI_ACTIVE_THEME->color_01_hover);
		IMGUI_COLOR_CONFIG(ResizeGripActive, PFF_UI_ACTIVE_THEME->color_01_active);
		
		IMGUI_COLOR_CONFIG(Separator, ImVec4(0.45f, 0.45f, 0.45f, 1.0f));
		IMGUI_COLOR_CONFIG(SeparatorHovered, ImVec4(0.45f, 0.45f, 0.45f, 1.0f));
		IMGUI_COLOR_CONFIG(SeparatorActive, ImVec4(0.45f, 0.45f, 0.45f, 1.0f));
		
		IMGUI_COLOR_CONFIG(TitleBg, ImVec4(.0f, 0.25f, .0f, 1.0f));
		IMGUI_COLOR_CONFIG(TitleBgActive, ImVec4(.0f, 0.45f, .0f, 1.0f));
		IMGUI_COLOR_CONFIG(TitleBgCollapsed, ImVec4(.0f, 0.1f, .0f, 1.0f));
		
		IMGUI_COLOR_CONFIG(DockingPreview, ImVec4(0.1f, 0.4f, 0.1f, 1.0f));
		IMGUI_COLOR_CONFIG(MenuBarBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		IMGUI_COLOR_CONFIG(CheckMark, ImVec4(.0f, .625f, .0f, 1.0f));
		
		IMGUI_COLOR_CONFIG(SliderGrab, PFF_UI_ACTIVE_THEME->color_01_default);
		IMGUI_COLOR_CONFIG(SliderGrabActive, PFF_UI_ACTIVE_THEME->color_01_active);
		
		IMGUI_COLOR_CONFIG(ScrollbarBg, PFF_UI_ACTIVE_THEME->color_backbround);
		IMGUI_COLOR_CONFIG(ScrollbarGrab, PFF_UI_ACTIVE_THEME->color_01_default);
		IMGUI_COLOR_CONFIG(ScrollbarGrabHovered, PFF_UI_ACTIVE_THEME->color_01_hover);
		IMGUI_COLOR_CONFIG(ScrollbarGrabActive, PFF_UI_ACTIVE_THEME->color_01_active);
		
		IMGUI_COLOR_CONFIG(TextSelectedBg, ImVec4(0.0f, 0.2f, 0.0f, 1.0f));
		IMGUI_COLOR_CONFIG(Border, ImVec4(0.33f, 0.33f, 0.33f, 1.0f));
		IMGUI_COLOR_CONFIG(WindowBg, ImVec4(0.085f, 0.085f, 0.085f, 1.0f));
		IMGUI_COLOR_CONFIG(ChildBg, ImVec4(0.11f, 0.11f, 0.11f, 1.0f));
		IMGUI_COLOR_CONFIG(PopupBg, ImVec4(0.125f, 0.125f, 0.125f, 0.9f));

		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f
		IMGUI_STYLE_CONFIG(f32, FrameRounding, 2);
		IMGUI_STYLE_CONFIG(f32, WindowBorderSize, 0);
		IMGUI_STYLE_CONFIG(ImVec2, ItemSpacing, ImVec2(4, 4));
		IMGUI_STYLE_CONFIG(ImVec2, ItemInnerSpacing, ImVec2(4, 4));
		IMGUI_STYLE_CONFIG(ImVec2, WindowPadding, ImVec2(4, 4));

		// ItemInnerSpacing

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));

		// MenuBarBg
	}

	void imgui_layer::on_detach() {

		PFF_PROFILE_FUNCTION();

		LOG(Info, "detach imgui layer");
		
		config::save(config::file::editor, "UI", "font_size", m_font_size);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		CORE_LOG(Info, "detach");
	}

	void imgui_layer::on_update(const f32 delta_time) {
	}

	void imgui_layer::on_event(event& event) {

		PFF_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		if (event.is_in_category(EC_Mouse) && io.WantCaptureMouse) {

			event.handled = true;
			// CORE_LOG(Trace, "MOUSE EVENT")
		}
	}

	void imgui_layer::on_imgui_render() {

		PFF_PROFILE_FUNCTION();

		//show_performance();
	}

	void imgui_layer::show_performance() {

		ImGui::SetCurrentContext(m_context);
		// Demonstrate creating a simple static window with no decoration
		// + a context-menu to choose which corner of the screen to use.
		static int location = 1;
		{
			ImGuiWindowFlags window_flags = (ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoDocking |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_NoNav);
			if (location != -1)
				window_flags |= ImGuiWindowFlags_NoMove;

			if (show_fps) {

				set_next_window_pos(location);
				ImGui::SetNextWindowBgAlpha(0.8f); // Transparent background
				if (ImGui::Begin("performance_timer", &show_fps, window_flags)) {

					// Get the current style
					ImGuiStyle& style = ImGui::GetStyle();

					// Get the line spacing (vertical padding around text)
					f32 lineSpacing = style.ItemSpacing.y / 2;
					f32 fontSize = ImGui::GetFontSize();
					f32 work_percent = static_cast<f32>(m_work_time / (m_work_time + m_sleep_time));
					f32 sleep_percent = 1 - work_percent;
					char formattedText[32];
					ImVec2 curser_pos;
					ImVec2 textSize;

					ImGui::Text("performance timer");
					ImGui::Separator();
					if (m_limit_fps)
						ImGui::Text("current fps    %4d/%4d", m_current_fps, m_target_fps);
					else
						ImGui::Text("current fps    %4d", m_current_fps);

					// work_time
					ImGui::Text("work time  ");
					ImGui::SameLine();
					snprintf(formattedText, sizeof(formattedText), " %8.2f ", m_work_time);
					progressbar_with_text(work_percent, formattedText);
					ImGui::SameLine();
					ImGui::Text("ms");

					// sleep time
					ImGui::Text("sleep time ");
					ImGui::SameLine();
					snprintf(formattedText, sizeof(formattedText), " %8.2f ", m_sleep_time);
					progressbar_with_text(sleep_percent, formattedText);
					ImGui::SameLine();
					ImGui::Text("ms");


					if (ImGui::BeginPopupContextWindow()) {
						if (ImGui::MenuItem("custom", NULL, location == -1))
							location = -1;
						if (ImGui::MenuItem("center", NULL, location == -2))
							location = -2;
						if (ImGui::MenuItem("top-left", NULL, location == 0))
							location = 0;
						if (ImGui::MenuItem("top-right", NULL, location == 1))
							location = 1;
						if (ImGui::MenuItem("bottom-left", NULL, location == 2))
							location = 2;
						if (ImGui::MenuItem("bottom-right", NULL, location == 3))
							location = 3;
						if (&show_fps && ImGui::MenuItem("close"))
							show_fps = false;
						ImGui::EndPopup();
					}
				}
				ImGui::End();
			}
		}
	}

	ImFont* imgui_layer::get_font(const std::string& name) {

		if (!contains(m_fonts, name))
			return nullptr;

		return m_fonts.at(name);
	}

	void imgui_layer::begin_frame() {

		PFF_PROFILE_FUNCTION();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void imgui_layer::end_frame(VkCommandBuffer commandbuffer) {

		PFF_PROFILE_FUNCTION();

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

	void imgui_layer::capture_current_image(VkImageView frame_buffer) {

		PFF_PROFILE_FUNCTION();

		//m_Dset = ImGui_ImplVulkan_AddTexture(m_TextureSampler, frame_buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	}

	void imgui_layer::set_next_window_pos(int16 location) {

		PFF_PROFILE_FUNCTION();

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
		} else if (location == -2) {
			// Center window
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		}
	}

	void imgui_layer::progressbar_with_text(f32 percent, const char* text, f32 min_size_x, f32 min_size_y) {

		PFF_PROFILE_FUNCTION();

		ImVec2 curser_pos;
		ImVec2 textSize;

		curser_pos = ImGui::GetCursorScreenPos();
		ImGui::SetItemAllowOverlap();

		textSize = ImGui::CalcTextSize(text);
		textSize.x = std::max<f32>(textSize.x, min_size_x);
		textSize.y = std::max<f32>(textSize.y, min_size_y);

		ImGui::ProgressBar(percent, textSize, "");
		ImGui::SetCursorScreenPos(curser_pos);
		ImGui::Text("%s", text);
	}

}
