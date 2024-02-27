
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

// TEST 
#include "application.h"
#include "engine/render/renderer.h"
#include "engine/render/vk_swapchain.h"

#include "editor_layer.h"


namespace PFF {

	static void help_marker(const char* desc) {

		ImGui::TextDisabled(" ? ");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	editor_layer::~editor_layer() {

		LOG(Info, "Shutdown");
	}

	void editor_layer::on_attach() {

		m_swapchain_supported_presentmodes = application::get().get_renderer()->get_swapchain_suported_present_modes();
		for (auto mode : m_swapchain_supported_presentmodes)
			m_swapchain_supported_presentmodes_str.push_back(present_mode_to_str(mode));

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

		window_graphics_engine_settings();
		window_editor_settings();
		window_general_settings();

		ImGui::ShowDemoWindow();				// DEV-ONLY
	}


	void editor_layer::window_main_menu_bar() {

		const f32 titlebar_height = 60.f;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, titlebar_height));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav;
		// window_flags |= ImGuiWindowFlags_MenuBar;
		
		// ImGui::PushStyleColor(ImGuiCol_WindowBg, PFF_UI_ACTIVE_THEME->WindowBg);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::Begin("appliaction_titlebar", nullptr, window_flags);
		{

			ImGui::PopStyleVar(3);

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
			const f32 button_area_width = (button_width * 3) + (window_padding.x * 3);

			// tilebar drag area
			ImGui::SetCursorPos(ImVec2(window_padding.x, window_padding.y + titlebar_vertical_offset));

			// debug Drab bounds
			//fg_draw_list->AddRect(ImGui::GetCursorScreenPos(), ImVec2(w - button_area_width, titlebar_height), IM_COL32(222, 43, 43, 255));

#if 1	// FOR DEBUG VISAUL
			ImGui::InvisibleButton("##titlebar_drag_zone", ImVec2(w - button_area_width, titlebar_height - window_padding.y * 2));
#elif 0
			ImGui::Button("##titlebar_drag_zone", ImVec2(w - button_area_width, titlebar_height - window_padding.y * 2));
#endif // 1

			application::get().set_titlebar_hovered(ImGui::IsItemHovered());
			ImGui::SetItemAllowOverlap();


			ImGui::SetCursorPos(ImVec2(w - button_area_width + window_padding.x * 2, window_padding.y));
			if (ImGui::Button("Min", ImVec2(30.f, 30.f)))
				application::get().minimize_window();

			ImGui::SetCursorPos(ImVec2(w - button_area_width + window_padding.x * 3 + button_width, window_padding.y));
			if (ImGui::Button("Max", ImVec2(30.f, 30.f)))
				application::get().maximize_window();

			ImGui::SetCursorPos(ImVec2(w - button_area_width + window_padding.x * 4 + button_width * 2, window_padding.y));
			if (ImGui::Button("Close", ImVec2(30.f, 30.f)))
				application::get().close_application();


			window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			window_flags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize;

			ImGuiStyle* style = &ImGui::GetStyle();
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, style->Colors[ImGuiCol_WindowBg]);

			// make new window with menubar because I dont know how to limit the extend of a MenuBar
			// just ImGui::MenuBar() would bo over the entire width of [appliaction_titlebar]
			ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + 160, viewport->Pos.y + window_padding.y));
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::Begin("application_title_menubar", nullptr, window_flags);
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleVar(3);
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
						if (ImGui::MenuItem("General Settings"))
							m_show_general_settings = true;

						if (ImGui::MenuItem("Editor Settings"))
							m_show_editor_settings = true;

						if (ImGui::MenuItem("Graphics Engine Settings")) {

							m_show_graphics_engine_settings = true;
							LOG(Trace, "start GES window")
						}

						if (ImGui::BeginMenu("Color Theme")) {

							ImGui::Text("Select Theme");

							const char* items[] = { "Dark", "Light" };
							static int item_current_idx = 0;
							ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
							if (ImGui::BeginListBox("##Theme_selector", ImVec2(350, (ARRAY_SIZE(items) * ImGui::GetTextLineHeightWithSpacing()) - 1) )) {
								for (int n = 0; n < ARRAY_SIZE(items); n++) {
									const bool is_selected = (item_current_idx == n);
									if (ImGui::Selectable(items[n], is_selected)) {

										item_current_idx = n;
										UI::THEME::UI_theme = static_cast<UI::THEME::theme_selection>(item_current_idx);
										UI::THEME::update_UI_theme();
									}
								}
								ImGui::EndListBox();
							}
							ImGui::PopStyleVar();

							ImGui::Separator();
							ImGui::Text("change main-color");

							ImGuiColorEditFlags misc_flags = 0;
							static bool backup_color_init = false;
							static ImVec4 backup_color;
							if (!backup_color_init) {

								backup_color = UI::THEME::main_color;
								ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);
								backup_color_init = true;
							}


							// Generate a default palette. The palette will persist and can be edited.
							static bool saved_palette_init = true;
							static ImVec4 saved_palette[35] = {};
							if (saved_palette_init) {
								for (int n = 0; n < ARRAY_SIZE(saved_palette); n++) {
									ImGui::ColorConvertHSVtoRGB((n / 34.f), .8f, .8f,
										saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
									saved_palette[n].w = 1.0f; // Alpha
								}
								saved_palette_init = false;
							}

							if (ImGui::ColorPicker4("##picker", (float*)&UI::THEME::main_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview)) {
								UI::THEME::update_UI_colors(UI::THEME::main_color);
								UI::THEME::update_UI_theme();
							}

							ImGui::SameLine();
							ImGui::BeginGroup();

								ImGui::BeginGroup();
									ImGui::Text("Current");
									ImGui::ColorButton("##current", UI::THEME::main_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
								ImGui::EndGroup();

								ImGui::SameLine();
								ImGui::BeginGroup();
									ImGui::Text("Previous");
									if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40))) {

										UI::THEME::update_UI_colors(backup_color);
										UI::THEME::update_UI_theme();
									}
								ImGui::EndGroup();

								ImGui::Separator();
								ImGui::Text("Palette");
								for (int n = 0; n < ARRAY_SIZE(saved_palette); n++) {
									ImGui::PushID(n);
									if ((n % 5) != 0)
										ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

									ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
									if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(21, 21))) {

										UI::THEME::update_UI_colors(ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, UI::THEME::main_color.w));
										UI::THEME::update_UI_theme();
									}

									// Allow user to drop colors into each palette entry. Note that ColorButton() is already a
									// drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
									if (ImGui::BeginDragDropTarget()) {
										if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
											memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
										if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
											memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
										ImGui::EndDragDropTarget();
									}

									ImGui::PopID();
								}
							ImGui::EndGroup();
							ImGui::EndMenu();
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}
			}
			ImGui::End();

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
	}

	void editor_layer::window_general_debugger() {

		if (!m_show_general_debugger)
			return;
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar ;
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

	void editor_layer::window_graphics_engine_settings() {

		if (!m_show_graphics_engine_settings)
			return;

		ImVec2 topic_button_size = { 150, 30 };
		f32 default_item_width = 250;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 500, viewport->Size.y - 300), ImGuiCond_Once);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
		if (ImGui::Begin("Graphics Engine Settings", &m_show_graphics_engine_settings, window_flags)) {}

		const char* items[] = { "General_settings", "General_settings 02", "General_settings 03" };
		static u32 item_current_idx = 0;

		ImGuiStyle* style = &ImGui::GetStyle();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, style->Colors[ImGuiCol_WindowBg]);
		if (ImGui::BeginListBox("##Topic_selector", ImVec2(200, (ARRAY_SIZE(items) * ImGui::GetTextLineHeightWithSpacing()) - 1))) {

			ImGui::PopStyleColor();
			for (u32 n = 0; n < ARRAY_SIZE(items); n++) {

				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected))
					item_current_idx = n;
			}
			ImGui::EndListBox();

		}
		ImGui::PopStyleVar();

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		if (item_current_idx == 0) {		// [General_settings] in array [items]

			ImGui::BeginGroup();
			static u32 present_mode_selected = 0;
			if (ImGui::CollapsingHeader("Swapchain##GraphicsSettings", ImGuiTreeNodeFlags_DefaultOpen)) {

				static int selected = 1;

				if(ImGui::Button("?##Present_mode_popup"))
					ImGui::OpenPopup("present_mode_explanations");

				ImGui::SameLine();
				ImGui::SetNextItemWidth(default_item_width);
				if (ImGui::Combo("##Present_mode_selector", &selected, m_swapchain_supported_presentmodes_str.data(), m_swapchain_supported_presentmodes_str.size())) {

					LOG(Fatal, "NOT IMPLEMENTED YET")
				}
				ImGui::SameLine();
				ImGui::Text("Choose swapchain present mode of the [swapchain] for optimal performance and visual quality");


			}

			if (ImGui::BeginPopup("present_mode_explanations")) {

				ImGui::Text("Explenations for possible present modes:");
				ImGui::Separator();

				ImGui::Text("Immediate");
				ImGui::Indent(10.f);
				ImGui::Text("The presentation engine does not wait for a vertical blanking period to update the current image,");
				ImGui::Text("meaning this mode may result in visible tearing. No internal queuing of presentation requests is needed,");
				ImGui::Text("as the requests are applied immediately.");
				ImGui::Indent(-10.f);

				ImGui::Text("Mailbox");
				ImGui::Indent(10.f);
				ImGui::Text("The presentation engine waits for the next vertical blanking period to update the current image,");
				ImGui::Text("preventing tearing. It maintains a single-entry queue for pending presentation requests.");
				ImGui::Text("If the queue is full, new requests replace existing ones, ensuring timely processing during each vertical blanking period.");
				ImGui::Indent(-10.f);

				ImGui::Text("FiFo (First-In, First-Out)");
				ImGui::Indent(10.f);
				ImGui::Text("Similar to Mailbox, the presentation engine waits for the next vertical blanking period to update the current image,");
				ImGui::Text("avoiding tearing. Pending presentation requests are queued, with new requests added to the end and processed in order");
				ImGui::Text("during each non-empty blanking period.");
				ImGui::Indent(-10.f);

				ImGui::Text("Relaxed FiFo");
				ImGui::Indent(10.f);
				ImGui::Text("The presentation engine typically waits for the next vertical blanking period to update the current image.");
				ImGui::Text("However, if a blanking period has passed since the last update, it may update immediately, potentially resulting in tearing.");
				ImGui::Text("It uses a queue for pending presentation requests, ensuring timely processing.");
				ImGui::Indent(-10.f);

				ImGui::Text("Shared Demand Refresh");
				ImGui::Indent(10.f);
				ImGui::Text("The presentation engine and application share access to a single image.");
				ImGui::Text("The engine updates the image only after receiving a presentation request,");
				ImGui::Text("while the application must request updates as needed. Tearing may occur since updates can happen at any point.");
				ImGui::Indent(-10.f);

				ImGui::Text("Shared Continuous Refresh");
				ImGui::Indent(10.f);
				ImGui::Text("Both the presentation engine and application have access to a shared image.");
				ImGui::Text("The engine periodically updates the image on its regular refresh cycle without needing additional requests from the application.");
				ImGui::Text("However, if rendering is not timed correctly, tearing may occur.");
				ImGui::Indent(-10.f);

				ImGui::EndPopup();
			}


			ImGui::EndGroup();
		}

		if (item_current_idx == 0) {		// [General_settings] in array [items]

		}

		/*
		
		"immediate"
		;
		

		"mailbox"
		
		"the presentation engine waits for the next vertical blanking period to update the current image. "
		"Tearing cannot be observed. An internal single-entry queue is used to hold pending presentation requests. "
		"If the queue is full when a new presentation request is received, the new request replaces the existing entry, "
		"and any images associated with the prior entry become available for re-use by the application. One request is "
		"removed from the queue and processed during each vertical blanking period in which the queue is non-empty.";
		

		"FiFo"

		"the presentation engine waits for the next vertical blanking" 
		"period to update the current image. Tearing cannot be observed. An internal queue is used to"
		"hold pending presentation requests. New requests are appended to the end of the queue, and one" 
		"request is removed from the beginning of the queue and processed during each vertical blanking" 
		"period in which the queue is non-empty. This is the only value of presentMode that is required" 
		"to be supported."


		"Relaxed FiFo"

		"the presentation engine generally waits for the next vertical "
		"blanking period to update the current image. If a vertical blanking period has already passed "
		"since the last update of the current image then the presentation engine does not wait for "
		"another vertical blanking period for the update, meaning this mode may result in visible tearing "
		"in this case. This mode is useful for reducing visual stutter with an application that will "
		"mostly present a new image before the next vertical blanking period, but may occasionally be "
		"late, and present a new image just after the next vertical blanking period. An internal queue "
		"is used to hold pending presentation requests. New requests are appended to the end of the queue, "
		"and one request is removed from the beginning of the queue and processed during or after each "
		"vertical blanking period in which the queue is non-empty"
		

		"Shared Demand Refresh"

		"the presentation engine and application have "
		"concurrent access to a single image, which is referred to as a shared presentable image. "
		"The presentation engine is only required to update the current image after a new presentation "
		"request is received. Therefore the application must make a presentation request whenever an "
		"update is required. However, the presentation engine may update the current image at any point, "
		"meaning this mode may result in visible tearing."


		"shared continuous refresh"
		
		"The presentation engine and application have "
		"concurrent access to a single image, which is referred to as a shared presentable image. The "
		"presentation engine periodically updates the current image on its regular refresh cycle. The "
		"application is only required to make one initial presentation request, after which the "
		"presentation engine must update the current image without any need for further presentation "
		"requests. The application can indicate the image contents have been updated by making a "
		"presentation request, but this does not guarantee the timing of when it will be updated. "
		"This mode may result in visible tearing if rendering to the image is not timed correctly."
		
		*/
		
		
		ImGui::End();
	}

	void editor_layer::window_editor_settings() {
	}

	void editor_layer::window_general_settings() {
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