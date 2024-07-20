
#include <util/pch_editor.h>

//#include <imgui.h>

//#include "util/io/serializer.h"

#include "util/ui/pannel_collection.h"
#include "engine/platform/pff_window.h"
#include "engine/layer/imgui_layer.h"

#include "toolkit/todo_list/todo_list.h"
#include "toolkit/settings/graphics_engine_settings.h"
#include "toolkit/texture_editor.h"

// TEST 
#include "application.h"
//#include "engine/render/renderer.h"
//#include "engine/render/vk_swapchain.h"

#include "editor_layer.h"


namespace PFF {

	editor_layer::editor_layer(ImGuiContext* context) : layer("editor_layer"), m_context(context) { LOG_INIT(); }

	editor_layer::~editor_layer() { LOG_SHUTDOWN(); }

	void editor_layer::on_attach() {

		LOG(Trace, "attaching editor_layer");
		// inform GLFW window to hide title_bar
		application::get().get_window()->show_titlebar(false);

		//m_swapchain_supported_presentmodes = application::get().get_renderer()->get_swapchain_suported_present_modes();
		//for (auto mode : m_swapchain_supported_presentmodes)
		//	m_swapchain_supported_presentmodes_str.push_back(present_mode_to_str(mode));

		//s_todo_list = new toolkit::todo::todo_list();
	}

	void editor_layer::on_detach() {

		for (auto& editor_window : m_editor_windows) 
			editor_window.release();
		m_editor_windows.clear();

		//application::get().get_window()->show_titlebar(true);
		//toolkit::todo::shutdown();					// only need to cal shutdown() to kill todo_list if editor shutsdown
		//delete s_todo_list;
		LOG(Trace, "Detaching editor_layer");
	}

	void editor_layer::on_update(f32 delta_time) { }

	void editor_layer::on_event(event& event) { }

	void editor_layer::on_imgui_render() {
		
		ImGui::SetCurrentContext(m_context);

		window_main_title_bar();
		window_main_content();
		
		window_general_debugger();
		window_outliner();
		window_details();
		window_world_settings();
		window_content_browser_0();
		window_content_browser_1();

		window_graphics_engine_settings();
		window_editor_settings();
		window_general_settings();

		PFF::toolkit::settings::window_graphics_engine();
		PFF::toolkit::todo::window_todo_list();

		for (const auto& editor_window : m_editor_windows)
			editor_window->window();

		if (style_editor)
			ImGui::ShowStyleEditor();

		if (demo_window)
			ImGui::ShowDemoWindow();
	}


	void editor_layer::window_main_title_bar() {

		const f32 m_titlebar_height = 60.f;

		ImGuiStyle* style = &ImGui::GetStyle();
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, m_titlebar_height));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav;
		
		auto color_buf = style->Colors[ImGuiCol_Button];
		auto main_color = IM_COL32(color_buf.x * 255, color_buf.y * 255, color_buf.z * 255, color_buf.w * 255);
		auto BG_color = IM_COL32(20, 20, 20, 255);

		// ImGui::PushStyleColor(ImGuiCol_WindowBg, PFF_UI_ACTIVE_THEME->WindowBg);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, BG_color);

		ImGui::Begin("appliaction_titlebar", nullptr, window_flags);

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);

		const bool is_maximized = application::get_window()->is_maximized();
		float titlebar_vertical_offset = is_maximized ? 6.f : 0.f;

		ImGui::SetCursorPos(ImVec2(0.f, titlebar_vertical_offset));
		ImVec2 titlebar_min = ImGui::GetCursorScreenPos();
		const ImVec2 titlebar_max = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth(),
										ImGui::GetCursorScreenPos().y + m_titlebar_height };

		/*auto* bg_draw_list = ImGui::GetBackgroundDrawList();
		auto* fg_draw_list = ImGui::GetForegroundDrawList();*/
		// bg_draw_list->AddRectFilled(titlebar_min, titlebar_max, IM_COL32(21, 251, 21, 255));

		auto* window_draw_list = ImGui::GetWindowDrawList();
		//window_draw_list->AddRectFilled(uperleft_corner, lowerright_corner, IM_COL32(51, 255, 51, 255));

		window_draw_list->AddRectFilled(titlebar_min, { titlebar_min.x + 200.f, titlebar_max.y }, main_color);
		titlebar_min.x += 200.f;
		window_draw_list->AddRectFilledMultiColor(titlebar_min, { titlebar_min.x + 550.f, titlebar_max.y }, main_color, BG_color, BG_color, main_color);

		// Debug titlebar bounds
		//fg_draw_list->AddRect(titlebar_min, titlebar_max, IM_COL32(222, 43, 43, 255));

		static f32 move_offset_y;
		static f32 move_offset_x;
		const f32 w = ImGui::GetContentRegionAvail().x;
		const f32 button_width = 25.f;
		const f32 button_spaccing = 8.f;
		const f32 button_area_width = viewport->Size.x - ((button_spaccing * 3) + (button_width * 3));

		// tilebar drag area
		ImGui::SetCursorPos(ImVec2(0.f, titlebar_vertical_offset));

#if 1	// FOR DEBUG VISAUL
		ImGui::InvisibleButton("##titlebar_drag_zone", ImVec2(button_area_width, m_titlebar_height));
#else
		ImGui::Button("##titlebar_drag_zone", ImVec2(button_area_width, m_titlebar_height));
#endif // 1

		application::get().set_titlebar_hovered(ImGui::IsItemHovered());
		ImGui::SetItemAllowOverlap();

		// LOGO
		{

		}

		const ImVec2 window_padding = { style->WindowPadding.x / 2,style->WindowPadding.y + 3.f };

		ImGui::SetItemAllowOverlap();
		ImGui::PushFont(application::get().get_imgui_layer()->get_font("giant"));
		ImGui::SetCursorPos(ImVec2(25, ((m_titlebar_height - ImGui::GetFontSize() + titlebar_vertical_offset) / 2)));
		ImGui::Text("PFF Editor");
		ImGui::PopFont();

		ImGui::SetCursorPos(ImVec2(viewport->Size.x - (window_padding.x + (button_spaccing * 2) + (button_width * 3)), window_padding.y + titlebar_vertical_offset));
		if (ImGui::Button("_##Min", ImVec2(button_width, button_width)))
			application::get().minimize_window();

		ImGui::SetCursorPos(ImVec2(viewport->Size.x - (window_padding.x + (button_spaccing * 1) + (button_width * 2)), window_padding.y + titlebar_vertical_offset));
		if (ImGui::Button("O##Max", ImVec2(button_width, button_width))) {
			if (is_maximized)
				application::get().restore_window();
			else
				application::get().maximize_window();
		}

		ImGui::SetCursorPos(ImVec2(viewport->Size.x - (window_padding.x + button_width), window_padding.y + titlebar_vertical_offset));
		if (ImGui::Button("X##Close", ImVec2(button_width, button_width)))
			application::get().close_application();

		// make new window with menubar because I dont know how to limit the extend of a MenuBar
		// just ImGui::MenuBar() would bo over the entire width of [appliaction_titlebar]
		{
			ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + 160, viewport->Pos.y + window_padding.y + titlebar_vertical_offset));
			ImGui::SetNextWindowSize({ 0,0 });
			ImGui::SetNextWindowViewport(viewport->ID);
			window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			window_flags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize;
			
			//ImGuiStyle* style = &ImGui::GetStyle();
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0, 0, 0, 0 });
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0, 0, 0, 0 });

			ImGui::Begin("application_title_menubar", nullptr, window_flags);

				ImGui::PopStyleColor(2);
				ImGui::PopStyleVar(3);
				main_menu_bar();

			ImGui::End();
		}

		ImGui::End();
	}


	void editor_layer::window_general_debugger() {

		if (!m_show_general_debugger)
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("Editor Debugger", &m_show_general_debugger, window_flags)) {

			if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {

				f32 tab_width = 60.f;		// TODO: move into [default_tab_width] variable in config-file
				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Inputs")) {

									
					UI::begin_table("display_input_actions_params", false);
					for (input_action* action : *application::get().get_world_layer()->get_current_player_controller()->get_input_mapping()) {						// get input_action

						switch (action->value) {
						case input::action_type::boolean:
							UI::table_row(action->get_name(), action->data.boolean, ImGuiInputTextFlags_ReadOnly);
							break;

						case input::action_type::vec_1D:
							UI::table_row(action->get_name(), action->data.vec_1D, ImGuiInputTextFlags_ReadOnly);
							break;

						case input::action_type::vec_2D:
							UI::table_row(action->get_name(), action->data.vec_2D, ImGuiInputTextFlags_ReadOnly);
							break;

						case input::action_type::vec_3D:
							UI::table_row(action->get_name(), action->data.vec_3D, ImGuiInputTextFlags_ReadOnly);
							break;

						default:
							break;
						}
					}
					UI::end_table();
					

					ImGui::EndTabItem();
				}

				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Test")) {

					// camera pos and dir
					if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {

						//static_cast<PFF_editor>(application::get()); .get_editor_layer();
						//glm::vec3 camera_pos = get_editor_camera_pos();
						UI::begin_table("##Camera_params", false);

						UI::table_row("Position", glm::vec3(), 0);
						UI::table_row("Direction", glm::vec2(), 0);

						UI::end_table();

					}

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

		}
		ImGui::End();
	}


	void editor_layer::window_main_content() {

		auto viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + m_titlebar_height)/*, ImGuiCond_Once*/);
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - m_titlebar_height));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags host_window_flags = 0;
		host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
		host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		//if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		//	host_window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("main_content_area", NULL, host_window_flags);
		ImGui::PopStyleVar(3);

		static ImGuiDockNodeFlags dockspace_flags = 0 /* | ImGuiDockNodeFlags_AutoHideTabBar*/;
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		
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

	void editor_layer::window_graphics_engine_settings() { }

	void editor_layer::window_editor_settings() { }

	void editor_layer::window_general_settings() { }

	void editor_layer::main_menu_bar() {

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
				if (ImGui::MenuItem("Quit", "Alt + F4"))
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

				ImGui::MenuItem("Editor Settings", "", &m_show_editor_settings);

				ImGui::MenuItem("Graphics Engine", "", &PFF::toolkit::settings::s_show_graphics_engine_settings);

				if (ImGui::BeginMenu("UI")) {

					UI::big_text("Select Theme");

					const char* items[] = { "Dark", "Light" };
					static int item_current_idx = 0;
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
					if (ImGui::BeginListBox("##Theme_selector", ImVec2(350, (ARRAY_SIZE(items) * ImGui::GetTextLineHeightWithSpacing()) - 1))) {
						for (int n = 0; n < ARRAY_SIZE(items); n++) {
							const bool is_selected = (item_current_idx == n);
							if (ImGui::Selectable(items[n], is_selected)) {

								item_current_idx = n;
								UI::set_UI_theme_selection(static_cast<UI::theme_selection>(item_current_idx));
								CORE_LOG(Debug, "updating UI theme" << (int)UI::UI_theme);
								UI::update_UI_theme();
							}
						}
						ImGui::EndListBox();
					}
					ImGui::PopStyleVar();

					// =============================== Init all vars just once ===============================
					ImGuiColorEditFlags misc_flags = 0;
					static bool backup_color_init = false;
					static bool window_border = false;
					static ImVec4 backup_color;
					if (!backup_color_init) {

						serializer::yaml(config::get_filepath_from_configtype(config::file::ui), "window_border", serializer::option::load_from_file)
							.entry(KEY_VALUE(window_border));
						//config::load(config::file::editor, "UI", "window_border", window_border);

						backup_color = UI::main_color;
						ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);
						backup_color_init = true;
					}

					if (ImGui::Checkbox("Window Border", &window_border))
						UI::enable_window_border(window_border);

					ImGui::Separator();
					ImGui::Text("change main-color");

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

					if (ImGui::ColorPicker4("##picker", (float*)&UI::main_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
						UI::update_UI_colors(UI::main_color);

					ImGui::SameLine();
					ImGui::BeginGroup();
					{
						ImGui::BeginGroup();
						{
							ImGui::Text("Current");
							ImGui::ColorButton("##current", UI::main_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
						}
						ImGui::EndGroup();

						ImGui::SameLine();
						{
							ImGui::BeginGroup();
							ImGui::Text("Previous");
							if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
								UI::update_UI_colors(backup_color);
						}
						ImGui::EndGroup();

						ImGui::Separator();
						ImGui::Text("Palette");
						for (int n = 0; n < ARRAY_SIZE(saved_palette); n++) {
							ImGui::PushID(n);
							if ((n % 5) != 0)
								ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

							ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
							if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(21, 21)))
								UI::update_UI_colors(ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, UI::main_color.w));

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
					}
					ImGui::EndGroup();

					//static f32 background_brightness = 0;
					//ImGui::SetNextItemWidth(350);
					//UI::begin_table("background_settings_values", false, ImVec2(350, (ImGui::GetTextLineHeightWithSpacing() * 2)) );
					//UI::table_row_slider("brightness", background_brightness);
					//UI::end_table();

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows")) {

				ImGui::MenuItem("ToDo List", "", &PFF::toolkit::todo::s_show_todo_list);
				ImGui::MenuItem("Style Editor", "", &style_editor);
				ImGui::MenuItem("Demo Window", "", &demo_window);

				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Tools")) {

				if (ImGui::MenuItem("Texture Editor", "", nullptr)) {

					LOG(Trace, "Adding Editor Window (texture_editor)");
					m_editor_windows.emplace_back( std::make_unique<toolkit::texture_editor>() );
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
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