
#include <util/pch_editor.h>

#include "util/ui/pannel_collection.h"
#include "engine/platform/pff_window.h"
#include "engine/layer/imgui_layer.h"

#include "toolkit/todo_list/todo_list.h"
#include "toolkit/texture_editor.h"
#include "ui/engine_wiki.h"
#include "settings/graphics_engine_settings.h"
#include "settings/editor_settings_window.h"

// TEST 
#include "application.h"
#include "PFF_editor.h"
#include "engine/render/renderer.h"

#include "editor_layer.h"


namespace PFF {


	editor_layer::editor_layer(ImGuiContext* context) : layer("editor_layer"), m_context(context) { LOG_INIT(); }


	editor_layer::~editor_layer() { LOG_SHUTDOWN(); }


	void editor_layer::on_attach() {

		LOG(Trace, "attaching editor_layer");

		const std::filesystem::path icon_path = util::get_executable_path() / "assets" / "icons";
#define LOAD_ICON(name)			m_##name##_icon = create_ref<image>(icon_path / #name ".png", image_format::RGBA)
		LOAD_ICON(folder);
		LOAD_ICON(folder_open);
		LOAD_ICON(folder_add);
		LOAD_ICON(folder_big);
		LOAD_ICON(world);
		LOAD_ICON(warning);
		LOAD_ICON(mesh_asset);
		LOAD_ICON(texture);
		LOAD_ICON(material);
		LOAD_ICON(material_inst);
		LOAD_ICON(relation);
		LOAD_ICON(file);
		LOAD_ICON(file_proc);
		LOAD_ICON(mesh_mini);
		LOAD_ICON(show);
		LOAD_ICON(hide);
#undef	LOAD_ICON

		m_transfrom_translation_image	= create_ref<image>(icon_path / "transfrom_translation.png", image_format::RGBA);
		m_transfrom_rotation_image		= create_ref<image>(icon_path / "transfrom_rotation.png", image_format::RGBA);
		m_transfrom_scale_image			= create_ref<image>(icon_path / "transfrom_scale.png", image_format::RGBA);

		m_world_viewport_window = new world_viewport_window();

		// inform GLFW window to hide title_bar
		application::get().get_window()->show_titlebar(false);
		
		serialize(serializer::option::load_from_file);
	}


	void editor_layer::on_detach() {

		delete m_world_viewport_window;

		m_folder_icon.reset();
		m_folder_big_icon.reset();
		m_folder_add_icon.reset();
		m_folder_big_icon.reset();
		m_world_icon.reset();
		m_warning_icon.reset();
		m_mesh_asset_icon.reset();

		m_texture_icon.reset();
		m_material_icon.reset();
		m_material_inst_icon.reset();
		m_relation_icon.reset();

		m_file_icon.reset();
		m_file_proc_icon.reset();
		m_mesh_mini_icon.reset();
		m_show_icon.reset();
		m_hide_icon.reset();
		m_transfrom_translation_image.reset();
		m_transfrom_rotation_image.reset();
		m_transfrom_scale_image.reset();

		serialize(serializer::option::save_to_file);

		for (auto& editor_window : m_editor_windows)
			editor_window.reset();
		m_editor_windows.clear();

		LOG(Trace, "Detaching editor_layer");
	}


	void editor_layer::on_update(f32 delta_time) {
	
		// First pass to mark items for removal
		auto it = std::remove_if(m_editor_windows.begin(), m_editor_windows.end(),
			[](const scope_ref<PFF::editor_window>& editor_window) {
				return editor_window->should_close();
			});

		// Erase the removed items
		m_editor_windows.erase(it, m_editor_windows.end());
	}


	void editor_layer::on_event(event& event) { }


	const char* delete_popup_lable;
	const char* delete_popup_explanation;
	std::function<void()> delete_popup_delete_action;

	// void delete_popup() {
			
	// 	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	// 	if (ImGui::BeginPopupModal("Confirm Delete", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

	// 		ImGui::Text("Are you sure you want to delete this directory?\nThis action cannot be undone.");
	// 		ImGui::Separator();

	// 		if (ImGui::Button("Yes", ImVec2(120, 0))) {

	// 			delete_popup_delete_action();
				
	// 			ImGui::CloseCurrentPopup();
	// 		}
	// 		ImGui::SameLine();
	// 		if (ImGui::Button("No", ImVec2(120, 0))) {

	// 			ImGui::CloseCurrentPopup();
	// 		}

	// 		ImGui::EndPopup();
	// 	}
	// }
	
	void editor_layer::show_delete_popup() {

		ImGui::OpenPopup("editor_delete_popup");
		LOG(Trace, "Opening editor_delete_popup");
		m_show_delete_popup = true;
		// delete_popup_lable = lable;
		// delete_popup_explanation = explanation;
		// delete_popup_delete_action = delete_action;
	}

	void editor_layer::on_imgui_render() {
		
		ImGui::SetCurrentContext(m_context);

		window_main_title_bar();
		window_main_content();

		m_world_viewport_window->window();

		settings::window_editor_settings(&m_show_editor_settings);
		window_general_settings();								// TODO: convert into editor window
		PFF::toolkit::settings::window_graphics_engine();		// TODO: convert into editor window
		PFF::toolkit::todo::window_todo_list();					// TODO: convert into editor window
		PFF::UI::engine_wiki_window();							// TODO: convert into editor window

		// First pass to mark items for removal
		for (const auto& editor_window : m_editor_windows)
			editor_window->window();

#ifdef PFF_DEBUG
		if (style_editor)
			ImGui::ShowStyleEditor();

		if (demo_window)
			ImGui::ShowDemoWindow();
#endif

		// // Always center this window when appearing
		// ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		// ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		// if (ImGui::BeginPopupModal("editor_delete_popup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

		// 	ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!");
		// 	ImGui::Separator();

		// 	//static int unused_i = 0;
		// 	//ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

		// 	static bool dont_ask_me_next_time = false;
		// 	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		// 	ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
		// 	ImGui::PopStyleVar();

		// 	if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		// 	ImGui::SetItemDefaultFocus();
		// 	ImGui::SameLine();
		// 	if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		// 	ImGui::EndPopup();
		// }

	}



	void editor_layer::serialize(serializer::option option) {

		serializer::yaml(config::get_filepath_from_configtype(application::get().get_project_path(), config::file::editor), "windows_to_show", option)
			.entry("show_main_menu_bar", m_show_main_menu_bar)
			.entry("show_options", m_show_options)
			.entry("show_graphics_engine_settings", m_show_graphics_engine_settings)
			.entry("show_editor_settings", m_show_editor_settings)
			.entry("show_general_settings", m_show_general_settings)
			.entry("show_engine_wiki", PFF::UI::show_engine_wiki)
#ifdef PFF_DEBUG
			.entry("show_style_editor", style_editor)
			.entry("show_demo_window", demo_window)
#endif
			.entry("show_todo_lis", PFF::toolkit::todo::s_show_todo_list);
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

		auto* window_draw_list = ImGui::GetWindowDrawList();

		window_draw_list->AddRectFilled(titlebar_min, { titlebar_min.x + 200.f, titlebar_max.y }, main_color);
		titlebar_min.x += 200.f;
		window_draw_list->AddRectFilledMultiColor(titlebar_min, { titlebar_min.x + 550.f, titlebar_max.y }, main_color, BG_color, BG_color, main_color);

		static f32 move_offset_y;
		static f32 move_offset_x;
		const f32 w = ImGui::GetContentRegionAvail().x;
		const f32 button_width = 25.f;
		const f32 button_spaccing = 8.f;
		const f32 button_area_width = viewport->Size.x - ((button_spaccing * 3) + (button_width * 3));

		ImGui::SetCursorPos(ImVec2(0.f, titlebar_vertical_offset));														// tilebar drag area

#if 1	// FOR DEBUG VISAUL
		ImGui::InvisibleButton("##titlebar_drag_zone", ImVec2(button_area_width, m_titlebar_height));
#else
		ImGui::Button("##titlebar_drag_zone", ImVec2(button_area_width, m_titlebar_height));
#endif // 1

		application::get().set_is_titlebar_hovered(ImGui::IsItemHovered());
		ImGui::SetItemAllowOverlap();

		// LOGO
		{

		}

		const ImVec2 window_padding = { style->WindowPadding.x / 2, style->WindowPadding.y + 3.f };

		ImGui::SetItemAllowOverlap();
		ImGui::PushFont(application::get().get_imgui_layer()->get_font("giant"));
		ImGui::SetCursorPos(ImVec2(25, ((m_titlebar_height - ImGui::GetFontSize() + titlebar_vertical_offset) / 2)));
		ImGui::Text("PFF Editor");
		ImGui::PopFont();

		// display project title
		ImGui::PushFont(application::get().get_imgui_layer()->get_font("header_1"));
		const static auto project_name = application::get().get_project_data().display_name;
		const auto project_name_size = ImGui::CalcTextSize(project_name.c_str());
		ImGui::SetCursorPos(ImVec2(viewport->Size.x - (window_padding.x + (button_spaccing * 2) + (button_width * 3)) - project_name_size.x - 30, window_padding.y + titlebar_vertical_offset));
		ImGui::Text("%s", application::get().get_project_data().display_name.c_str());
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
		if (ImGui::Button("X##Close", ImVec2(button_width, button_width))) {

			LOG(Trace, "Titlebar close button clicked")
			application::get().close_application();
		}

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

			if (ImGui::BeginMenu("Settings")) {
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
								LOG(Debug, "updating UI theme" << (int)UI::UI_theme);
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

						serializer::yaml(config::get_filepath_from_configtype(application::get().get_project_path(), config::file::ui), "window_border", serializer::option::load_from_file)
							.entry(KEY_VALUE(window_border));
						//config::load(config::file::editor, "UI", "window_border", window_border);

						backup_color = UI::get_main_color_ref();
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

					if (ImGui::ColorPicker4("##picker", (float*)&UI::get_main_color_ref(), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
						UI::update_UI_colors(UI::get_main_color_ref());

					ImGui::SameLine();
					ImGui::BeginGroup();
					{
						ImGui::BeginGroup();
						{
							ImGui::Text("Current");
							ImGui::ColorButton("##current", UI::get_main_color_ref(), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
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
								UI::update_UI_colors(ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, UI::get_main_color_ref().w));

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
#ifdef PFF_DEBUG
				ImGui::MenuItem("Style Editor", "", &style_editor);
				ImGui::MenuItem("Demo Window", "", &demo_window);
#endif
				ImGui::MenuItem("Engine Wiki", "", &PFF::UI::show_engine_wiki);

				// IN DEV
				ImGui::SeparatorText("Settings");
				ImGui::MenuItem("Graphics Engine Settings", "", &m_show_graphics_engine_settings);
				ImGui::MenuItem("Editor Settings", "", &m_show_editor_settings);
				ImGui::MenuItem("General Settings", "", &m_show_general_settings);

				ImGui::Separator();		

				m_world_viewport_window->show_possible_sub_window_options();

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
