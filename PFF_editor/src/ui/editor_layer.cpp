
#include <util/pch_editor.h>

//#include <imgui.h>

//#include "util/io/serializer.h"
#include "util/ui/panels/pannel_collection.h"
#include "engine/platform/pff_window.h"
#include "util/color_theme.h"
#include "engine/layer/imgui_debug_layer.h"

#include "toolkit/todo_list/todo_list.h"
/*
// TEST 
#include "engine/render/renderer.h"
#include "engine/render/vk_swapchain.h"
*/

#include "editor_layer.h"


namespace PFF {

	editor_layer::editor_layer(ImGuiContext* context) : layer("editor_layer"), m_context(context) { LOG_INIT(); }

	editor_layer::~editor_layer() { LOG_SHUTDOWN(); }



	void editor_layer::on_attach() {

		m_context = ImGui::CreateContext();

		//m_swapchain_supported_presentmodes = application::get().get_renderer()->get_swapchain_suported_present_modes();
		//for (auto mode : m_swapchain_supported_presentmodes)
		//	m_swapchain_supported_presentmodes_str.push_back(present_mode_to_str(mode));

		//s_todo_list = new toolkit::todo::todo_list();
	}

	void editor_layer::on_detach() {

		LOG(Trace, "Detaching editor_layer");
		toolkit::todo::shutdown();					// only need to cal shutdown() to kill todo_list if editor shutsdown
		//delete s_todo_list;
	}

	void editor_layer::on_update(f32 delta_time) {
	}

	void editor_layer::on_event(event& event) {
	}

	void editor_layer::on_imgui_render() {

		ImGui::SetCurrentContext(m_context);

		//if (ImGui::Begin("Test_window")) {}
		//ImGui::End();


		//window_main_menu_bar();
		//// window_main_content();
		//
		//window_general_debugger();
		//window_outliner();
		//window_details();
		//window_world_settings();
		//window_content_browser_0();
		//window_content_browser_1();
		//
		//window_graphics_engine_settings();
		//window_editor_settings();
		//window_general_settings();
		//
		//PFF::toolkit::todo::window_todo_list();
				
		//ImGui::ShowDemoWindow();				// DEV-ONLY
	}


	void editor_layer::window_main_menu_bar() {

		const f32 titlebar_height = 60.f;

		ImGuiStyle* style = &ImGui::GetStyle();
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, titlebar_height));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav;
		// window_flags |= ImGuiWindowFlags_MenuBar;

		// ImGui::PushStyleColor(ImGuiCol_WindowBg, PFF_UI_ACTIVE_THEME->WindowBg);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::Begin("appliaction_titlebar", nullptr, window_flags);

		ImGui::PopStyleVar(3);

		const bool is_maximized = application::get_window()->is_maximized();
		float titlebar_vertical_offset = is_maximized ? 6.f : 0.f;

		ImGui::SetCursorPos(ImVec2(0.f, titlebar_vertical_offset));
		ImVec2 titlebar_min = ImGui::GetCursorScreenPos();
		const ImVec2 titlebar_max = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth(),
										ImGui::GetCursorScreenPos().y + titlebar_height };

		auto* bg_draw_list = ImGui::GetBackgroundDrawList();
		auto* fg_draw_list = ImGui::GetForegroundDrawList();
		// bg_draw_list->AddRectFilled(titlebar_min, titlebar_max, IM_COL32(21, 251, 21, 255));

		auto* window_draw_list = ImGui::GetWindowDrawList();
		//window_draw_list->AddRectFilled(uperleft_corner, lowerright_corner, IM_COL32(51, 255, 51, 255));

		auto color_buf = style->Colors[ImGuiCol_Button];
		ImVec4 bufsd = UI::THEME::main_titlebar_color;
		auto main_color = IM_COL32(color_buf.x * 255, color_buf.y * 255, color_buf.z * 255, color_buf.w * 255);

		window_draw_list->AddRectFilled(titlebar_min, { titlebar_min.x + 200.f, titlebar_max.y }, main_color);
		titlebar_min.x += 200.f;
		window_draw_list->AddRectFilledMultiColor(titlebar_min, { titlebar_min.x + 550.f, titlebar_max.y }, main_color, IM_COL32_BLACK_TRANS, IM_COL32_BLACK_TRANS, main_color);

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
		ImGui::InvisibleButton("##titlebar_drag_zone", ImVec2(button_area_width, titlebar_height));
#else
		ImGui::Button("##titlebar_drag_zone", ImVec2(button_area_width, titlebar_height));
#endif // 1

		application::get().set_titlebar_hovered(ImGui::IsItemHovered());
		ImGui::SetItemAllowOverlap();

		// LOGO
		{

		}

		const ImVec2 window_padding = { style->WindowPadding.x / 2,style->WindowPadding.y + 3.f };

		ImGui::SetItemAllowOverlap();
		//ImGui::PushFont(application::get().get_imgui_layer()->get_font("giant"));
		ImGui::SetCursorPos(ImVec2(25, ((titlebar_height - ImGui::GetFontSize() + titlebar_vertical_offset) / 2)));
		ImGui::Text("PFF Editor");
		//ImGui::PopFont();

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

		//ImGuiStyle* style = &ImGui::GetStyle();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0, 0, 0, 0 });

		// make new window with menubar because I dont know how to limit the extend of a MenuBar
		// just ImGui::MenuBar() would bo over the entire width of [appliaction_titlebar]
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + 160, viewport->Pos.y + window_padding.y + titlebar_vertical_offset));
		ImGui::SetNextWindowSize({ 0,0 });
		ImGui::SetNextWindowViewport(viewport->ID);
		{
			window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			window_flags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize;
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

					/*					
					UI::begin_default_table("display_input_actions_params");
					for (input_action* action : *application::get().get_world_layer()->get_current_player_controller()->get_input_mapping()) {						// get input_action

						switch (action->value) {
						case input::action_type::boolean:
							UI::add_table_row(action->get_name(), action->data.boolean, ImGuiInputTextFlags_ReadOnly);
							break;

						case input::action_type::vec_1D:
							UI::add_table_row(action->get_name(), action->data.vec_1D, ImGuiInputTextFlags_ReadOnly);
							break;

						case input::action_type::vec_2D:
							UI::add_table_row(action->get_name(), action->data.vec_2D, ImGuiInputTextFlags_ReadOnly);
							break;

						case input::action_type::vec_3D:
							UI::add_table_row(action->get_name(), action->data.vec_3D, ImGuiInputTextFlags_ReadOnly);
							break;

						default:
							break;
						}
					}
					UI::end_default_table();
					*/

					ImGui::EndTabItem();
				}

				ImGui::SetNextItemWidth(tab_width);
				if (ImGui::BeginTabItem("Test")) {

					// camera pos and dir
					if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {

						//static_cast<PFF_editor>(application::get()); .get_editor_layer();
						//glm::vec3 camera_pos = get_editor_camera_pos();
						UI::begin_default_table("##Camera_params");

						UI::add_table_row("Position", glm::vec3(), 0);
						UI::add_table_row("Direction", glm::vec2(), 0);

						UI::end_default_table();

					}

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

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 500, viewport->Size.y - 300), ImGuiCond_Once);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Graphics Engine Settings", &m_show_graphics_engine_settings, window_flags);
		ImGui::PopStyleVar();

		const f32 default_item_width = 250;
		const f32 first_width = 250.f;
		static u32 item_current_idx = 0;
		UI::custom_frame(first_width, [first_width]() mutable {

			const char* items[] = { "General_settings", "General_settings 02", "General_settings 03" };

			ImGuiStyle* style = &ImGui::GetStyle();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(25, style->FramePadding.y));
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(.5f, .5f));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, UI::THEME::highlited_window_bg);
			if (ImGui::BeginListBox("##Topic_selector", ImVec2(first_width, (ARRAY_SIZE(items) * ImGui::GetTextLineHeightWithSpacing()) - 1))) {

				for (u32 n = 0; n < ARRAY_SIZE(items); n++) {

					const bool is_selected = (item_current_idx == n);
					if (ImGui::Selectable(items[n], is_selected))
						item_current_idx = n;
				}
				ImGui::EndListBox();

			}
			ImGui::PopStyleColor();
			ImGui::PopStyleVar(3);

			}, [&] {

				if (item_current_idx == 0) {		// [General_settings] in array [items]

					ImGui::BeginGroup();
					static u32 present_mode_selected = 0;
					if (ImGui::CollapsingHeader("Swapchain##GraphicsSettings", ImGuiTreeNodeFlags_DefaultOpen)) {

						static int selected = 1;

						if (ImGui::Button(" ? ##Present_mode_popup"))
							ImGui::OpenPopup("present_mode_explanations");

						ImGui::SameLine();
						ImGui::SetNextItemWidth(UI::THEME::default_item_width);
						if (ImGui::Combo("##Present_mode_selector", &selected, m_swapchain_supported_presentmodes_str.data(), static_cast<int>(m_swapchain_supported_presentmodes_str.size()))) {

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
						ImGui::Text("The presentation engine does not wait for a vertical blanking period to update the current image,\n"
							"meaning this mode may result in visible tearing. No internal queuing of presentation requests is needed,\n"
							"as the requests are applied immediately.");
						ImGui::Indent(-10.f);

						ImGui::Text("Mailbox");
						ImGui::Indent(10.f);
						ImGui::Text("The presentation engine waits for the next vertical blanking period to update the current image,\n"
							"preventing tearing. It maintains a single-entry queue for pending presentation requests.\n"
							"If the queue is full, new requests replace existing ones, ensuring timely processing during each vertical blanking period.");
						ImGui::Indent(-10.f);

						ImGui::Text("FiFo (First-In, First-Out)");
						ImGui::Indent(10.f);
						ImGui::Text("Similar to Mailbox, the presentation engine waits for the next vertical blanking period to update the current image,\n"
							"avoiding tearing. Pending presentation requests are queued, with new requests added to the end and processed in order\n"
							"during each non-empty blanking period.");
						ImGui::Indent(-10.f);

						ImGui::Text("Relaxed FiFo");
						ImGui::Indent(10.f);
						ImGui::Text("The presentation engine typically waits for the next vertical blanking period to update the current image.\n"
							"However, if a blanking period has passed since the last update, it may update immediately, potentially resulting in tearing.\n"
							"It uses a queue for pending presentation requests, ensuring timely processing.");
						ImGui::Indent(-10.f);

						ImGui::Text("Shared Demand Refresh");
						ImGui::Indent(10.f);
						ImGui::Text("The presentation engine and application share access to a single image.\n"
							"The engine updates the image only after receiving a presentation request,\n"
							"while the application must request updates as needed. Tearing may occur since updates can happen at any point.");
						ImGui::Indent(-10.f);

						ImGui::Text("Shared Continuous Refresh");
						ImGui::Indent(10.f);
						ImGui::Text("Both the presentation engine and application have access to a shared image.\n"
							"The engine periodically updates the image on its regular refresh cycle without needing additional requests from the application.\n"
							"However, if rendering is not timed correctly, tearing may occur.");
						ImGui::Indent(-10.f);

						ImGui::EndPopup();
					}


					ImGui::EndGroup();
				}

				if (item_current_idx == 0) {		// [General_settings] in array [items]

				}

				});

		ImGui::End();
	}

	void editor_layer::window_editor_settings() {
	}

	void editor_layer::window_general_settings() {
	}

	void draw_todo_item(const char* title, const char* description) {

	}

	void editor_layer::set_next_window_pos(int16 location) {
	}

	void editor_layer::progressbar_with_text(f32 percent, const char* text, f32 min_size_x, f32 min_size_y) {
	}

	void editor_layer::progressbar(f32 percent, f32 min_size_x, f32 min_size_y) {
	}

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
					if (ImGui::BeginListBox("##Theme_selector", ImVec2(350, (ARRAY_SIZE(items) * ImGui::GetTextLineHeightWithSpacing()) - 1))) {
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

					// =============================== Init all vars just once ===============================
					ImGuiColorEditFlags misc_flags = 0;
					static bool backup_color_init = false;
					static bool window_border = false;
					static ImVec4 backup_color;
					if (!backup_color_init) {

						serializer::yaml(config::get_filepath_from_configtype(config::file::ui), "window_border", serializer::option::load_from_file)
							.entry(KEY_VALUE(window_border));
						//config::load(config::file::editor, "UI", "window_border", window_border);

						backup_color = UI::THEME::main_color;
						ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);
						backup_color_init = true;
					}

					if (ImGui::Checkbox("Window Border", &window_border))
						UI::THEME::enable_window_border(window_border);

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

					if (ImGui::ColorPicker4("##picker", (float*)&UI::THEME::main_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
						UI::THEME::update_UI_colors(UI::THEME::main_color);

					ImGui::SameLine();
					ImGui::BeginGroup();

					ImGui::BeginGroup();
					ImGui::Text("Current");
					ImGui::ColorButton("##current", UI::THEME::main_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
					ImGui::EndGroup();

					ImGui::SameLine();
					ImGui::BeginGroup();
					ImGui::Text("Previous");
					if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
						UI::THEME::update_UI_colors(backup_color);

					ImGui::EndGroup();

					ImGui::Separator();
					ImGui::Text("Palette");
					for (int n = 0; n < ARRAY_SIZE(saved_palette); n++) {
						ImGui::PushID(n);
						if ((n % 5) != 0)
							ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

						ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
						if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(21, 21)))
							UI::THEME::update_UI_colors(ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, UI::THEME::main_color.w));

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

			if (ImGui::BeginMenu("Windows")) {

				ImGui::MenuItem("ToDo List", "", &PFF::toolkit::todo::s_show_todo_list);

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