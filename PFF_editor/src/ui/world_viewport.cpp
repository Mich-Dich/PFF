
#include "util/pch_editor.h"

#include <imgui.h>
#include <entt.hpp>

#include "util/ui/pannel_collection.h"
#include "engine/world/map.h"

#include "engine/resource_management/static_mesh_asset_manager.h"
#include "engine/resource_management/mesh_serializer.h"

#include "world_viewport.h"

namespace PFF {

	world_viewport_window::world_viewport_window() {

		serialize(serializer::option::load_from_file);
	}

	world_viewport_window::~world_viewport_window() {

		serialize(serializer::option::save_to_file);
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

	void world_viewport_window::show_possible_sub_window_options() {

		ImGui::MenuItem("content browser 0", "", &m_show_content_browser_0);
		ImGui::MenuItem("content browser 1", "", &m_show_content_browser_1);
		ImGui::MenuItem("world settings", "", &m_show_world_settings);
		ImGui::MenuItem("general debugger", "", &m_show_general_debugger);
		ImGui::MenuItem("outliner", "", &m_show_outliner);
		ImGui::MenuItem("details", "", &m_show_details);
	}

	void world_viewport_window::serialize(serializer::option option) {

		serializer::yaml(config::get_filepath_from_configtype(application::get().get_project_path(), config::file::editor), "world_viewport_windows_to_show", option)
			.entry("show_renderer_backgrond_effect", m_show_renderer_backgrond_effect)
			.entry("show_general_debugger", m_show_general_debugger)
			.entry("show_outliner", m_show_outliner)
			.entry("show_details", m_show_details)
			.entry("show_world_settings", m_show_world_settings);

			// TODO: serialize selected entity (need figure out how to serialize the selected map)
			/*.sub_section("world_vieport_data", [selected_entity = m_selected_entity](serializer::yaml& section) {
				section.entry("selected entity", selected_entity);
			});*/
	}

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Sub windows
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void world_viewport_window::display_entity_children(ref<map> loc_map, entity& entity) {

		auto& relation_comp = entity.get_component<relationship_component>();
		for (const auto child_ID : relation_comp.children_ID) {

			PFF::entity child = loc_map->get_entity_by_UUID(child_ID);
			const char* name = child.get_component<tag_component>().tag.c_str();

			// Does have childer itself
			if (child.get_component<relationship_component>().children_ID.size() > static_cast<size_t>(0)) {

				ImGui::Text("O");		// TODO: replace with hide/show button
				ImGui::SameLine();
				const bool is_open = ImGui::TreeNodeEx(name,
					outliner_base_flags | ((m_selected_entity == child) ? ImGuiTreeNodeFlags_Selected : 0));
				
				if (ImGui::IsItemClicked())
					m_selected_entity = child;

				if (is_open) {

					display_entity_children(loc_map, child);
					ImGui::TreePop();
				}
				continue;
			}

			// Does not have more childer
			ImGui::Text("O");		// TODO: replace with hide/show button
			ImGui::SameLine();
			ImGui::TreeNodeEx(name,
				outliner_base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ((m_selected_entity == child) ? ImGuiTreeNodeFlags_Selected : 0));
			if (ImGui::IsItemClicked())
				m_selected_entity = child;
		}
	}

	void world_viewport_window::window_outliner() {

		if (!m_show_outliner)
			return;

		ImGuiWindowFlags window_flags{};
		ImGui::Begin("Outliner", &m_show_outliner, window_flags);

		const auto& maps = application::get().get_world_layer()->get_maps();
		for (const auto& loc_map : maps) {

			for (const auto entity_ID : loc_map->m_registry.view<entt::entity>()) {

				PFF::entity loc_entity = entity(entity_ID, loc_map.get());
				const auto& tag_comp = loc_entity.get_component<tag_component>();

				// has relationship
				if (loc_entity.has_component<relationship_component>()) {

					auto& relation_comp = loc_entity.get_component<relationship_component>();
					if (relation_comp.parent_ID != 0)	// skip all children in main display (will be displayed in [display_entity_children()])
						continue;

					ImGui::Text("O");		// TODO: replace with hide/show button
					ImGui::SameLine();
					const bool is_open = ImGui::TreeNodeEx(tag_comp.tag.c_str(),
						outliner_base_flags | ((m_selected_entity == loc_entity) ? ImGuiTreeNodeFlags_Selected : 0));
					if (ImGui::IsItemClicked())
						m_selected_entity = loc_entity;

					if (is_open) {

						display_entity_children(loc_map, loc_entity);
						ImGui::TreePop();
					}
					continue;
				}


				// has no relationship
				ImGui::Text("O");		// TODO: replace with hide/show button
				ImGui::SameLine();
				ImGui::TreeNodeEx(tag_comp.tag.c_str(),
					outliner_base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ((m_selected_entity == loc_entity) ? ImGuiTreeNodeFlags_Selected : 0));
				if (ImGui::IsItemClicked())
					m_selected_entity = loc_entity;

			}

		}

		// Reset m_selected_entity when clicking on empty space
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
			m_selected_entity = entity();

		ImGui::End();
	}


	void world_viewport_window::window_details() {

		if (!m_show_details)
			return;

		ImGuiWindowFlags window_flags{};
		if (!ImGui::Begin("Details", &m_show_details, window_flags))
			return;

		if (m_selected_entity == entity()) {

			ImGui::End();
			return;
		}

		// Component that every entity has
		if (ImGui::CollapsingHeader("Tag"), ImGuiTreeNodeFlags_DefaultOpen) {

			auto& tag_comp = m_selected_entity.get_component<tag_component>();
			UI::begin_table("entity_component", false);
			UI::table_row_text("tag", tag_comp.tag.c_str());
			UI::end_table();
		}

		// Component that every entity has
		if (ImGui::CollapsingHeader("Transform"), ImGuiTreeNodeFlags_DefaultOpen) {

			auto& transform_comp = m_selected_entity.get_component<transform_component>();
			UI::begin_table("entity_component", false);

			if (UI::table_row("transform", (glm::mat4&)transform_comp)) {		// TODO: change UI::table_row() to return true is transform was changed

				// TODO: propegate transform change if mobility::locked
				if (m_selected_entity.has_component<relationship_component>()) {

				}
			}

			UI::end_table(); 
		}
		
		if (m_selected_entity.has_component<mesh_component>()) {

			// Component that every entity has
			if (ImGui::CollapsingHeader("Mesh"), ImGuiTreeNodeFlags_DefaultOpen) {

				auto& mesh_comp = m_selected_entity.get_component<mesh_component>();
				UI::begin_table("entity_component", false);
				
				UI::table_row([]() {
					ImGui::Text("mobility");
				}, [&]() {

					static const char* items[] = { "locked", "movable", "dynamic" };
					static int item_current_idx = static_cast<std::underlying_type_t<mobility>>(mesh_comp.mobility);
					const char* combo_preview_value = items[item_current_idx];
					static ImGuiComboFlags flags = 0;
					if (ImGui::BeginCombo("##details_window_mesh_component_mobility", combo_preview_value, flags)) {
						
						for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
							const bool is_selected = (item_current_idx == n);
							if (ImGui::Selectable(items[n], is_selected))
								item_current_idx = n;

							if (is_selected)		// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}
				});
				UI::table_row("shoudl render", mesh_comp.shoudl_render);

				UI::table_row([]() {
					ImGui::Text("mesh asset");
				}, [&]() {

					ImGui::Text(mesh_comp.asset_path.string().c_str());

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FILE")) {

							const std::filesystem::path file_path = (const char*)payload->Data;
							mesh_comp.asset_path = util::extract_path_from_project_content_folder(file_path);
							mesh_comp.mesh_asset = static_mesh_asset_manager::get_from_path(mesh_comp.asset_path);
						}
						ImGui::EndDragDropTarget();
					}
					
				});

				UI::end_table();
			}
		}


		ImGui::End();
	}

	void world_viewport_window::window_general_debugger() {

		if (!m_show_general_debugger)
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("Editor Debugger", &m_show_general_debugger, window_flags)) {

			if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {

				const f32 tab_width = 60.f;		// TODO: move into [default_tab_width] variable in config-file
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
		const bool is_window_begin = ImGui::Begin("Viewport##PFF_Engine", nullptr, window_flags);
		ImGui::PopStyleVar(2);

		if (!is_window_begin) {

			ImGui::End();
			return;
		}

		// display rendred image
		const ImVec2 viewport_size = ImGui::GetContentRegionAvail();
		application::get().get_renderer().set_imugi_viewport_size(glm::u32vec2(viewport_size.x, viewport_size.y));

		auto* draw_image = application::get().get_renderer().get_draw_image_pointer();
		const ImVec2 viewport_uv = {	math::clamp(viewport_size.x / draw_image->get_width(), 0.f, 1.f),
										math::clamp(viewport_size.y / draw_image->get_height(), 0.f, 1.f) };
		ImGui::Image(draw_image->get_descriptor_set(), ImVec2{ viewport_size.x, viewport_size.y }, ImVec2{ 0,0 }, viewport_uv);

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

			ImGui::EndPopup();
		}


		if (ImGui::BeginDragDropTarget()) {

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FILE")) {

				const std::filesystem::path file_path = (const char*)payload->Data;
				asset_file_header loc_asset_file_header;
				if (file_path.extension() == ".pffasset") {

					serializer::binary(file_path, "PFF_asset_file", serializer::option::load_from_file)
						.entry(loc_asset_file_header);

				} else if (file_path.extension() == ".pffworld") {

					serializer::yaml(file_path, "PFF_asset_file", serializer::option::load_from_file)
						.entry(KEY_VALUE(loc_asset_file_header.version))
						.entry(KEY_VALUE(loc_asset_file_header.type))
						.entry(KEY_VALUE(loc_asset_file_header.timestamp));
				}

				switch (loc_asset_file_header.type) {
					
				case file_type::mesh: {

					CORE_LOG(Trace, "Adding static mesh, Name: " << "SM_" + file_path.filename().replace_extension("").string());

					mesh_component mesh_comp{};
					mesh_comp.asset_path = util::extract_path_from_project_content_folder(file_path);

					if (m_selected_entity != entity()) {

						m_selected_entity.add_mesh_component(mesh_comp);
						break;
					}
					
					const auto loc_map = application::get().get_world_layer()->get_maps()[0];
					entity loc_entitiy = loc_map->create_entity("SM_" + file_path.filename().replace_extension("").string());
						
					loc_entitiy.add_mesh_component(mesh_comp);

					//mesh_comp.mesh_asset = static_mesh_asset_manager::get_from_path(util::extract_path_from_project_content_folder(file_path));
					//mesh_comp.material = GET_RENDERER.get_default_material_pointer();		// get correct shader

				} break;

				default:
					break;
				}

			}
			ImGui::EndDragDropTarget();

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

				UI::table_row_slider<int>("Effects", background_effect_index, 0, application::get().get_renderer().get_number_of_background_effects() - 1.f);

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
