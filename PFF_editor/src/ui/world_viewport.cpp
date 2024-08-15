
#include "util/pch_editor.h"

#include <imgui.h>

#include "util/ui/pannel_collection.h"

#include "world_viewport.h"

namespace PFF {

	world_viewport_window::world_viewport_window() {

	}

	world_viewport_window::~world_viewport_window() {

	}

	void world_viewport_window::window() {

		ImGuiWindowFlags window_flags = 0//ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoCollapse;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		bool is_window_begin = ImGui::Begin("World###PFF_main_world_window", nullptr, window_flags);	// TODO: get title of world
		ImGui::PopStyleVar(2);

		if (is_window_begin) {

			ImGuiID dockspace_id = ImGui::GetID("EditorDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

			window_general_debugger();								// TODO: convert into editor window
			window_world_settings();								// TODO: convert into editor window
			window_main_viewport();

			m_content_browser.window();

			window_outliner();
			window_details();

		}
		ImGui::End();

	}

	void world_viewport_window::serialize(serializer::option option) {

		serializer::yaml(config::get_filepath_from_configtype(application::get().get_project_path(), config::file::editor), "windows_to_show", option)
			.entry("show_renderer_backgrond_effect", m_show_renderer_backgrond_effect)
			.entry("show_general_debugger", m_show_general_debugger)
			.entry("show_outliner", m_show_outliner)
			.entry("show_details", m_show_details)
			.entry("show_world_settings", m_show_world_settings);
	}

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Sub windows
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void world_viewport_window::window_outliner() {

		if (!m_show_outliner)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("Outliner", &m_show_outliner, window_flags)) {}




		ImGui::End();
	}


	void world_viewport_window::window_details() {

		if (!m_show_details)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("Details", &m_show_details, window_flags)) {}

		ImGui::End();
	}

	void world_viewport_window::window_general_debugger() {

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

	void world_viewport_window::window_main_viewport() {

		ImGuiWindowFlags window_flags = 0//ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoBackground
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_DockNodeHost;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		bool is_window_begin = ImGui::Begin("Viewport##PFF_Engine", nullptr, window_flags);
		ImGui::PopStyleVar(2);

		if (is_window_begin) {

			// display rendred image
			ImVec2 viewport_size = ImGui::GetContentRegionAvail();

			application::get().get_renderer().set_imugi_viewport_size(glm::u32vec2(viewport_size.x, viewport_size.y));

			auto* buffer = application::get().get_renderer().get_draw_image_pointer();

			ImVec2 viewport_uv = {
				std::max(std::min(viewport_size.x / buffer->get_width(), 1.f), 0.f),
				std::max(std::min(viewport_size.y / buffer->get_height(), 1.f), 0.f) };
			ImGui::Image(buffer->get_descriptor_set(), ImVec2{ viewport_size.x, viewport_size.y }, ImVec2{ 0,0 }, viewport_uv);

			// show debug data
			application::get().get_imgui_layer()->show_FPS();
			application::get().get_imgui_layer()->show_renderer_metrik();
			window_renderer_backgrond_effect();

			if (ImGui::BeginPopupContextWindow()) {

				ImGui::Text("Performance Analysis");
				ImGui::Separator();
				ImGui::Checkbox("Show FPS window", application::get().get_imgui_layer()->get_show_FPS_window_pointer());
				ImGui::Checkbox("Show renderer metrik", application::get().get_imgui_layer()->get_show_renderer_metrik_pointer());
				ImGui::Checkbox("Show render background settings", &m_show_renderer_backgrond_effect);
				ImGui::Separator();

				ImGui::EndPopup();
			}

		}
		ImGui::End();
	}

	void world_viewport_window::window_renderer_backgrond_effect() {

		if (!m_show_renderer_backgrond_effect)
			return;

		static UI::window_pos location = UI::window_pos::bottom_left;

		ImGuiWindowFlags window_flags = (
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav);
		if (location != UI::window_pos::custom)
			window_flags |= ImGuiWindowFlags_NoMove;

		UI::set_next_window_pos_in_window(location);
		ImGui::SetNextWindowBgAlpha(0.8f); // Transparent background
		if (ImGui::Begin("Render Debug", &m_show_renderer_backgrond_effect, window_flags)) {

			render::compute_effect& selected = application::get().get_renderer().get_current_background_effect();
			int& background_effect_index = application::get().get_renderer().get_current_background_effect_index();

			if (UI::begin_table("renderer background values", true, ImVec2(300, 0), 0, true, 0.3f)) {

				UI::table_row_slider("Effects", background_effect_index, 0, application::get().get_renderer().get_number_of_background_effects() - 1);

				if (background_effect_index == 0) {}

				else if (background_effect_index == 1) {

					UI::table_row_slider("top color", selected.data.data1);
					UI::table_row_slider("bottom color", selected.data.data2);

				} else if (background_effect_index == 2) {

					UI::table_row_slider<glm::vec3>("bottom color", (glm::vec3&)selected.data.data1);
					UI::table_row_slider<f32>("star amount", selected.data.data1[3]);

				} else {

					UI::table_row_slider("data 1", selected.data.data1);
					UI::table_row_slider("data 2", selected.data.data2);
					UI::table_row_slider("data 3", selected.data.data3);
					UI::table_row_slider("data 4", selected.data.data4);
				}

				UI::end_table();
			}

			UI::next_window_position_selector_popup(location, m_show_renderer_backgrond_effect);
		}
		ImGui::End();
	}

	void world_viewport_window::window_world_settings() {

		if (!m_show_world_settings)
			return;

		ImGuiWindowFlags window_flags{};
		if (ImGui::Begin("World Settings", &m_show_world_settings, window_flags)) {}

		ImGui::End();
	}



}
