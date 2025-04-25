
#include "util/pch_editor.h"

#include <entt/entt.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

#include "PFF_editor.h"

#include "util/ui/pannel_collection.h"
#include "ui/component_UI.h"
#include "engine/world/map.h"
#include "engine/resource_management/mesh_serializer.h"

#include "content_browser.h"
#include "ui/logger_window.h"

#include "world_viewport.h"

namespace PFF {

	static logger_window m_logger_window;

	
	world_viewport_window::world_viewport_window() {

		serialize(serializer::option::load_from_file);

		auto* editor_layer = PFF_editor::get().get_editor_layer();
		m_transfrom_translation_image	= editor_layer->get_transfrom_translation_image();
		m_transfrom_rotation_image		= editor_layer->get_transfrom_rotation_image();
		m_transfrom_scale_image			= editor_layer->get_transfrom_scale_image();
		m_folder_add_icon				= editor_layer->get_folder_add_icon();
		m_mesh_asset_icon				= editor_layer->get_mesh_asset_icon();
		m_relation_icon					= editor_layer->get_relation_icon();
		m_file_icon						= editor_layer->get_file_icon();
		m_file_proc_icon				= editor_layer->get_file_proc_icon();
		m_mesh_mini_icon				= editor_layer->get_mesh_mini_icon();
		m_show_icon						= editor_layer->get_hide_icon();
		m_hide_icon						= editor_layer->get_show_icon();

		m_logger_window					= logger_window{};
	}


	world_viewport_window::~world_viewport_window() {

		m_transfrom_translation_image.reset();
		m_transfrom_rotation_image.reset();
		m_transfrom_scale_image.reset();
		m_folder_add_icon.reset();
		m_mesh_asset_icon.reset();
		m_relation_icon.reset();
		m_file_icon.reset();
		m_mesh_mini_icon.reset();
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

			if (m_show_log_display)
				m_logger_window.window();

			window_outliner();
			window_details();

		}
		ImGui::End();


		m_deletion_queue.flush();
	}


	void world_viewport_window::show_possible_sub_window_options() {

		ImGui::MenuItem("log display", "", &m_show_log_display);
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
			.entry("show_log_display", m_show_world_settings)
			.entry("show_world_settings", m_show_log_display);

		serializer::yaml(config::get_filepath_from_configtype(application::get().get_project_path(), config::file::editor), "guizmo_data", option)
			.entry("operation", m_gizmo_operation);



		if (option == serializer::option::save_to_file && m_selected_entitys.main_item == entity())
			return;

		UUID selected_ID{};
		if (option == serializer::option::save_to_file) {

			selected_ID = m_selected_entitys.main_item.get_UUID();
			LOG(Trace, "Save UUID: " << selected_ID);
		}

		serializer::yaml(config::get_filepath_from_configtype(application::get().get_project_path(), config::file::editor), "world_vieport_data", option)
			.entry("selected_entity", selected_ID);

		//if (option == serializer::option::load_from_file) {

		//	// TODO: load selected entity after loading world/map
		//	LOG(Trace, "Load UUID: " << selected_ID);
		//	if (auto loc_entity = application::get().get_world_layer()->get_map()->get_entity_by_UUID(selected_ID)) {

		//		m_selected_entitys.main_item = loc_entity;
		//		LOG(Info, "FOUND UUID: " << selected_ID);
		//	}
		//}

	}

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Sub windows
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void world_viewport_window::add_show_hide_icon(PFF::entity& entity) {

		if (entity.has_component<mesh_component>()) {

			mesh_component& mesh_comp = entity.get_component<mesh_component>();
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(240, 240, 240, 20));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0));
			
			if (ImGui::ImageButton(((mesh_comp.shoudl_render) ? m_hide_icon->get_descriptor_set() : m_show_icon->get_descriptor_set()), ImVec2(15)))
				mesh_comp.shoudl_render = !mesh_comp.shoudl_render;

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::SameLine();
		}
	}


	void world_viewport_window::display_entity_children(ref<map> loc_map, PFF::entity entity) {

		const auto& name_comp = entity.get_component<name_component>();
		const auto& relationship_comp = entity.get_component<relationship_component>();
		for (size_t x = 0; x < relationship_comp.children_ID.size(); x++) {
			
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			
			PFF::entity child = loc_map->get_entity_by_UUID(relationship_comp.children_ID[x]);
			add_show_hide_icon(child);

			const auto& child_name_comp = child.get_component<name_component>();
			const auto& child_relationship_comp = child.get_component<relationship_component>();
			if (child_relationship_comp.children_ID.empty()) {

				ImGui::TreeNodeEx(child_name_comp.name.c_str(), ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ((m_selected_entitys.main_item == child) ? ImGuiTreeNodeFlags_Selected : 0));
				if (ImGui::IsItemClicked())
					m_selected_entitys.main_item = child;

				ImGui::TableNextColumn();
				list_all_components(child);

				continue;
			}

			const auto flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAllColumns;
			const bool is_open = ImGui::TreeNodeEx(child_name_comp.name.c_str(), flags | ((m_selected_entitys.main_item == child) ? ImGuiTreeNodeFlags_Selected : 0));
			if (ImGui::IsItemClicked())
				m_selected_entitys.main_item = child;

			ImGui::TableNextColumn();
			list_all_components(child);

			if (is_open) {

				display_entity_children(loc_map, child);
				ImGui::TreePop();
			}
		}
	}


	void world_viewport_window::list_all_components(PFF::entity entity) {

		//ImGui::SameLine();

		if (entity.has_component<relationship_component>()) {

			ImGui::Image(m_relation_icon->get_descriptor_set(), ImVec2(15));
			ImGui::SameLine();
		}
		
		if (entity.has_component<mesh_component>()) {

			ImGui::Image(m_mesh_mini_icon->get_descriptor_set(), ImVec2(15));
			ImGui::SameLine();
		}
		
		if (entity.has_component<script_component>()) {

			ImGui::Image(m_file_icon->get_descriptor_set(), ImVec2(15));
			ImGui::SameLine();
		}
		
		if (entity.has_component<procedural_mesh_component>()) {

			ImGui::Image(m_file_proc_icon->get_descriptor_set(), ImVec2(15));
			ImGui::SameLine();
		}

		ImGui::NewLine();
	}


	void world_viewport_window::window_outliner() {

		if (!m_show_outliner)
			return;

		bool is_any_item_hovered = false;

		ImGuiWindowFlags window_flags{};
		ImGui::Begin("Outliner", &m_show_outliner, window_flags);

		static std::string search_query;
		UI::serach_input("##serach_in_world_viewport_details_panel", search_query);

		ImGui::SameLine();
		if (ImGui::ImageButton(m_folder_add_icon->get_descriptor_set(), ImVec2{ 15 }, ImVec2(0), ImVec2(1))) {

			LOG(Trace, "Add folder to outliner: NOT IMPLEMENTED YET");
		}

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const auto style = ImGui::GetStyle();
		const ImVec2 pos_offset = ImVec2(style.WindowPadding.x, 0);
		const f32 pos_x = ImGui::GetCursorScreenPos().x - style.WindowPadding.x;
		const ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeightWithSpacing()) + pos_offset;

		//UI::shift_cursor_pos(20, 0);
		static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
		ImGui::BeginTable("world outliner", 2, flags);
		ImGui::TableSetupColumn("entity", ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("components", ImGuiTableColumnFlags_WidthFixed, 70.f);
		ImGui::TableHeadersRow();

		UI::mouse_interation item_mouse_interation = UI::mouse_interation::none;
		const auto& map_ref = application::get().get_world_layer()->get_map();
		for (const auto entity_ID : map_ref->m_registry.view<entt::entity>()) {

			PFF::entity loc_entity = entity(entity_ID, map_ref.get());
			const auto& name_comp = loc_entity.get_component<name_component>();
			const ImVec4& color = (loc_entity == m_selected_entitys.main_item) ? UI::get_action_color_00_active_ref() : (m_selected_entitys.item_set.find(loc_entity) != m_selected_entitys.item_set.end()) ? UI::get_action_color_00_faded_ref() : UI::get_action_color_gray_hover_ref(); // ImGui::GetStyle().Colors[ImGuiCol_ChildBg];

			// has relationship
			if (loc_entity.has_component<relationship_component>()) {

				auto& relation_comp = loc_entity.get_component<relationship_component>();
				if (relation_comp.parent_ID != 0)	// skip all children in main display (will be displayed in [display_entity_children()])
					continue;

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				add_show_hide_icon(loc_entity);
				
				//std::string item_name = "outliner_entity_" + index++;
				//ImGui::PushID(item_name.c_str());
				const auto flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAllColumns;
				ImGui::PushStyleColor(ImGuiCol_Header, color);
				const bool is_open = ImGui::TreeNodeEx(name_comp.name.c_str(), flags | ((m_selected_entitys.main_item == loc_entity) ? ImGuiTreeNodeFlags_Selected : 0));
				item_mouse_interation = UI::get_mouse_interation_on_item();
				ImGui::PopStyleColor();
				//ImGui::PopID();

				ImGui::TableNextColumn();
				list_all_components(loc_entity);

				if (is_open) {

					display_entity_children(map_ref, loc_entity);
					ImGui::TreePop();
				}

				continue;
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			add_show_hide_icon(loc_entity);
			
			ImGui::PushStyleColor(ImGuiCol_Header, color);
			ImGui::TreeNodeEx(name_comp.name.c_str(), ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ((m_selected_entitys.main_item == loc_entity) ? ImGuiTreeNodeFlags_Selected : 0));
			item_mouse_interation = UI::get_mouse_interation_on_item();
			ImGui::PopStyleColor();

			util::process_selection_input<PFF::entity>(m_selected_entitys, item_mouse_interation, loc_entity,"outliner_entity_context_menu", []() {

				LOG(Warn, "Not implemented yet")
			});

			//switch (item_mouse_interation) {
			//	// case UI::mouse_interation::left_clicked:	m_selected_entitys.main_item = loc_entity; break;
			//	case UI::mouse_interation::right_clicked:	ImGui::OpenPopup("outliner_entity_context_menu"); break;
			//	
			//	case UI::mouse_interation::left_pressed:

			//		if (ImGui::GetIO().KeyShift) {
			//			
			//			if (m_selected_entitys.main_item.is_valid()) {										// If main item selected -> perform range selection.

			//				// TODO: need to perform multi selection somehow

			//			} else
			//				m_selected_entitys.main_item = loc_entity;										// If main item not selected -> simply mark clicked item as main selection.
			//				
			//		} else if (ImGui::GetIO().KeyCtrl) {

			//			
			//			if (m_selected_entitys.main_item.is_valid())
			//				m_selected_entitys.item_set.insert(m_selected_entitys.main_item);
			//			m_selected_entitys.main_item = loc_entity;

			//			// if (m_selected_entitys.item_set.find(loc_entity) == m_selected_entitys.item_set.end())				// If Shift is held, select the item
			//			// 	m_selected_entitys.item_set.insert(loc_entity);
			//			// else
			//			// 	m_selected_entitys.item_set.erase(loc_entity);

			//		} else {

			//			m_selected_entitys.item_set.clear();
			//			m_selected_entitys.main_item = loc_entity;
			//		}
			//	break;
			//	default: break;
			//}

			ImGui::TableNextColumn();
			list_all_components(loc_entity);
		}

		ImGui::EndTable();

		// Reset m_selected_entitys.main_item when clicking on empty space
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
			m_selected_entitys.main_item = entity();

		if (ImGui::BeginPopupContextItem("outliner_entity_context_menu")) {

			if (ImGui::MenuItem("Rename entity"))
				LOG(Info, "NOT IMPLEMENTED YET");

			if (ImGui::MenuItem("Delete entity")) {								// open popup to display consequences of deleting file and ask again

				if (!m_selected_entitys.main_item.has_component<relationship_component>()) {

					LOG(Info, "need to cleanup realtionships, TODO: add settings to decide if children will also be deleted or just reparented")
				}

				if (m_selected_entitys.main_item)
					m_deletion_queue.push_func([&]() {
						if (!m_selected_entitys.main_item.has_component<relationship_component>()) {

							LOG(Info, "need to cleanup realtionships, TODO: add settings to decide if children will also be deleted or just reparented")
						}
						const auto& map_ref = application::get().get_world_layer()->get_map();
						map_ref->destroy_entity(m_selected_entitys.main_item);
						m_selected_entitys.main_item = PFF::entity{};
					});
			}

			// const auto interaction = UI::get_mouse_interation_on_window();
			// if (UI::get_mouse_interation_on_window() == UI::mouse_interation::none && !UI::is_holvering_window())
			// 	ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		ImGui::End();
	}


	void world_viewport_window::window_details() {

		if (!m_show_details)
			return;

		ImGuiWindowFlags window_flags{};
		if (!ImGui::Begin("Details", &m_show_details, window_flags))
			return;

		const auto button_size = ImGui::CalcTextSize("Add Component").x;

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - button_size - (ImGui::GetStyle().ItemSpacing.x * 3));
		std::string search_text = "";
		if (UI::serach_input("##serach_in_world_viewport_details_panel", search_text)) {

			LOG(Info, "Not implemented yet");
		}

		ImGui::SameLine();
		if (ImGui::Button("Add Component"), button_size) {

			// LOG(Info, "Not implemented yet");
		}

		if (m_selected_entitys.main_item == entity()) {

			ImGui::Text("No entity selected");
			ImGui::End();
			return;
		}

		UI::display_name_comp(m_selected_entitys.main_item);
		UI::display_transform_comp(m_selected_entitys.main_item, PFF_editor::get().get_editor_settings_ref().display_rotator_in_degrees);
		UI::try_display_mesh_comp(m_selected_entitys.main_item);
		UI::try_display_procedural_script_comp(m_selected_entitys.main_item);
		UI::try_display_relationship_comp(m_selected_entitys.main_item);

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

						//UI::table_row(static_cast<std::string_view>("Position"), glm::vec3(), .0f);
						//UI::table_row("Direction", glm::vec2(), .0f);

						UI::end_table();

					}

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

		}
		ImGui::End();
	}


	void world_viewport_window::process_drop_of_file(const std::filesystem::path path, const bool set_as_selected_entity) {

		std::filesystem::path absolute_path = application::get().get_project_path() / CONTENT_DIR / path;
		asset_file_header loc_asset_file_header;
		if (path.extension() == ".pffasset") {

			serializer::binary(absolute_path, "PFF_asset_file", serializer::option::load_from_file)
				.entry(loc_asset_file_header);

		} else if (path.extension() == ".pffworld") {

			serializer::yaml(absolute_path, "PFF_asset_file", serializer::option::load_from_file)
				.entry(KEY_VALUE(loc_asset_file_header.file_version))
				.entry(KEY_VALUE(loc_asset_file_header.type))
				.entry(KEY_VALUE(loc_asset_file_header.timestamp));
		}

		switch (loc_asset_file_header.type) {

		case file_type::mesh: {

			LOG(Trace, "Adding static mesh, Name: " << "SM_" + path.filename().replace_extension("").string());

			mesh_component mesh_comp{};
			mesh_comp.asset_path = path;

			if (m_selected_entitys.main_item != entity()) {

				m_selected_entitys.main_item.add_mesh_component(mesh_comp);
				break;
			}

			const auto loc_map = application::get().get_world_layer()->get_map();
			entity loc_entitiy = loc_map->create_entity("SM_" + path.filename().replace_extension("").string());
			loc_entitiy.add_mesh_component(mesh_comp);

			if (set_as_selected_entity)
				m_selected_entitys.main_item = loc_entitiy;

			//mesh_comp.mesh_asset = static_mesh_asset_manager::get_from_path(util::extract_path_from_project_content_folder(path));
			//mesh_comp.material = GET_RENDERER.get_default_material_pointer();		// get correct shader

		} break;

		default:
			break;
		}
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
		const ImVec2 viewport_uv = { math::clamp(viewport_size.x / draw_image->get_width(), 0.f, 1.f),
										math::clamp(viewport_size.y / draw_image->get_height(), 0.f, 1.f) };
		ImGui::Image(draw_image->get_descriptor_set(), ImVec2{ viewport_size.x, viewport_size.y }, ImVec2{ 0,0 }, viewport_uv);

		if (ImGui::BeginDragDropTarget()) {

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DRAG_DROP_CONTENT_BROWSER_MESH)) {

				const std::filesystem::path file_path = (const char*)payload->Data;
				process_drop_of_file(file_path, true);
				// m_selected_entitys.main_item = 
			}

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DRAG_DROP_CONTENT_BROWSER_MULTI)) {

				util::selected_items<std::filesystem::path>** received_ptr = static_cast<util::selected_items<std::filesystem::path>**>(payload->Data);
				util::selected_items<std::filesystem::path>* file_paths = *received_ptr;
				for (const std::filesystem::path path : (*file_paths).item_set)
					process_drop_of_file(path, false);

				process_drop_of_file(file_paths->main_item, true);
			}

			ImGui::EndDragDropTarget();
		}


		auto* draw_list = ImGui::GetWindowDrawList();

		const auto start_pos = ImVec2(ImGui::GetWindowWidth() - 100, 5);

		const ImVec2 button_size = ImVec2(15); // size of the button
		const ImVec2 box_padding = ImVec2(5); // padding around the button
		const f32 button_length = button_size.x + (box_padding.x * 2);
		ImVec4 color = (m_gizmo_operation == transform_operation::translate) ? ImGui::GetStyle().Colors[ImGuiCol_Button] : UI::get_default_gray_ref();
		const ImU32 color_u32 = ImGui::ColorConvertFloat4ToU32(color);
		{
			const ImVec2 box_min = ImGui::GetWindowPos() + start_pos; // top-left corner
			const ImVec2 box_max = box_min + button_size + (box_padding * 2); // bottom-right corner
			const f32 corner_size = 7;

			draw_list->AddRectFilled(box_min, ImVec2(box_max.x, box_max.y - corner_size), color_u32);
			draw_list->AddRectFilled(ImVec2(box_min.x + corner_size, box_max.y - corner_size), box_max, color_u32);
			draw_list->AddTriangleFilled(ImVec2(box_min.x, box_max.y - corner_size),
				ImVec2(box_min.x + corner_size, box_max.y - corner_size),
				ImVec2(box_min.x + corner_size, box_max.y), color_u32);

			ImGui::SetCursorPos(start_pos + box_padding);
			if (ImGui::ImageButton(m_transfrom_translation_image->get_descriptor_set(), ImVec2{ 15 }, ImVec2(0), ImVec2(1), 0, color)) {

				LOG(Trace, "TRANSLATE button");
				m_gizmo_operation = transform_operation::translate;
			}
		}

		color = m_gizmo_operation == transform_operation::rotate ? ImGui::GetStyle().Colors[ImGuiCol_Button] : UI::get_default_gray_ref();
		{
			const ImVec2 box_min = ImGui::GetWindowPos() + start_pos + ImVec2(button_length, 0);
			const ImVec2 box_max = box_min + button_size + (box_padding * 2);

			draw_list->AddRectFilled(box_min, box_max, ImGui::ColorConvertFloat4ToU32(color));

			ImGui::SetCursorPos(start_pos + ImVec2(button_length, 0) + box_padding);
			if (ImGui::ImageButton(m_transfrom_rotation_image->get_descriptor_set(), ImVec2{ 15 }, ImVec2(0), ImVec2(1), 0, color)) {

				LOG(Trace, "ROTATE button");
				m_gizmo_operation = transform_operation::rotate;
			}
		}

		color = m_gizmo_operation == transform_operation::scale ? ImGui::GetStyle().Colors[ImGuiCol_Button] : UI::get_default_gray_ref();
		{
			const ImVec2 box_min = ImGui::GetWindowPos() + start_pos + ImVec2(button_length * 2, 0);
			const ImVec2 box_max = box_min + button_size + (box_padding * 2);

			draw_list->AddRectFilled(box_min, box_max, ImGui::ColorConvertFloat4ToU32(color));

			ImGui::SetCursorPos(start_pos + ImVec2(button_length * 2, 0) + box_padding);
			if (ImGui::ImageButton(m_transfrom_scale_image->get_descriptor_set(), ImVec2{ 15 }, ImVec2(0), ImVec2(1), 0, color)) {

				LOG(Trace, "SCALE button");
				m_gizmo_operation = transform_operation::scale;
			}
		}



		// show debug data
		application::get().get_imgui_layer()->show_FPS();
		application::get().get_imgui_layer()->show_renderer_metrik();
		window_renderer_backgrond_effect();

		if (UI::get_mouse_interation_on_window() == UI::mouse_interation::right_double_clicked)
			ImGui::OpenPopup("viewport additional functionality");
		if (ImGui::BeginPopup("viewport additional functionality")) {

			ImGui::Text("Performance Analysis");
			ImGui::Separator();
			ImGui::Checkbox("Show FPS window", application::get().get_imgui_layer()->get_show_FPS_window_pointer());
			ImGui::Checkbox("Show renderer metrik", application::get().get_imgui_layer()->get_show_renderer_metrik_pointer());
			ImGui::Checkbox("Show render background settings", &m_show_renderer_backgrond_effect);

			ImGui::EndPopup();
		}

		// ---------------------------------------- IGuizmo ----------------------------------------
		if (m_selected_entitys.main_item) {

			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (f32)ImGui::GetWindowWidth(), (f32)ImGui::GetWindowHeight());

			ref<camera> camera = application::get().get_world_layer()->get_editor_camera();
			glm::mat4& view_matrix = camera->get_view_NC();

			glm::mat4& projection_matrix = camera->get_projection_NC();
			projection_matrix[2][2] = -projection_matrix[2][2];
			projection_matrix[3][2] = -projection_matrix[3][2];

			glm::mat4& entity_transform = (glm::mat4&)m_selected_entitys.main_item.get_component<transform_component>();
			glm::mat4 buffer_transform = entity_transform;

			if (ImGuizmo::Manipulate(glm::value_ptr(view_matrix), glm::value_ptr(projection_matrix),
				(ImGuizmo::OPERATION)m_gizmo_operation, ImGuizmo::MODE::WORLD, glm::value_ptr(entity_transform))) {

				const glm::mat4 root_transform = buffer_transform;
				buffer_transform = glm::inverse(buffer_transform) * entity_transform;		// trandform delta
				m_selected_entitys.main_item.propegate_transform_to_children(root_transform, buffer_transform);
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

			int& background_effect_index = application::get().get_renderer().get_current_background_effect_index();

			UI::begin_table("renderer background values", true, ImVec2(300, 0), 0, true, 0.3f);

			UI::table_row_slider<int>("Effects", background_effect_index, 0, 3);

			if (background_effect_index < 3) {

				render::compute_effect& selected = application::get().get_renderer().get_current_background_effect();
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

			} else if (background_effect_index == 3) {

				ImGui::Text("Not implemented yet");
			}

			UI::end_table();
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
