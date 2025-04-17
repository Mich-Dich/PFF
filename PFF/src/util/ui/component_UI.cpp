
#include "util/pffpch.h"

#include "util/ui/pannel_collection.h"
#include "util/data_structures/path_manipulation.h"
#include "engine/resource_management/headers.h"
#include "engine/resource_management/general_resource_manager.h"
#include "engine/resource_management/static_mesh_asset_manager.h"
#include "project/script_system.h"
#include "application.h"
#include "engine/layer/world_layer.h"
#include "engine/world/map.h"
#include "engine/world/entity.h"

#include "component_UI.h"

namespace PFF::UI {

	void try_display_mesh_comp(PFF::entity entity) {

		UI::try_display_component<mesh_component>("Mesh", entity, [](auto& component) {

			UI::table_row([]() { ImGui::Text("mesh asset"); }, [&]() {

				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
				const auto comp_asset_path = component.asset_path.filename().string();
				ImGui::Text("%s", comp_asset_path.c_str());

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_ASSET_MESH")) {

						const std::filesystem::path file_path = (const char*)payload->Data;
						bool file_currupted = false;
						PFF::resource_manager::asset_curruption_source loc_asset_curruption_source = PFF::resource_manager::asset_curruption_source::unknown;
						PFF::asset_file_header loc_asset_file_header;
						file_currupted = resource_manager::try_to_deserialize_file_header(file_path, true, loc_asset_curruption_source, loc_asset_file_header);
				
						if (!file_currupted && loc_asset_file_header.type == PFF::file_type::mesh) {

							component.asset_path = util::extract_path_from_project_content_folder(file_path);
							component.mesh_asset = static_mesh_asset_manager::get_from_path(component.asset_path);
						} else
							LOG(Warn, "Tryed to drop a material-instance asset but provided asset")
					}
					ImGui::EndDragDropTarget();
				}
			});

			UI::table_row([]() { ImGui::Text("mobility"); }, [&]() {

				static const char* items[] = { "locked", "movable", "dynamic" };
				static int item_current_idx = static_cast<std::underlying_type_t<mobility>>(component.mobility_data);
				const char* combo_preview_value = items[item_current_idx];
				static ImGuiComboFlags flags = 0;
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
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
			
			UI::table_row("shoudl render", component.shoudl_render);

			UI::table_row([]() { ImGui::Text("material instance"); }, [&]() {

				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
				const auto comp_asset_path = component.material_inst_path.filename().string();
				if (comp_asset_path.empty())
					ImGui::Text("%s", comp_asset_path.c_str());
				else
					ImGui::Text("no material instance selected");

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_ASSET_MATERIAL_INST")) {

						const std::filesystem::path file_path = (const char*)payload->Data;

						// TODO: check if asset is a material instance
						bool file_currupted = false;
						PFF::resource_manager::asset_curruption_source loc_asset_curruption_source = PFF::resource_manager::asset_curruption_source::unknown;
						PFF::asset_file_header loc_asset_file_header;
						file_currupted = resource_manager::try_to_deserialize_file_header(file_path, true, loc_asset_curruption_source, loc_asset_file_header);
				
						if (!file_currupted && loc_asset_file_header.type == PFF::file_type::material_instance) {

							component.material_inst_path = util::extract_path_from_project_content_folder(file_path);
							// component.material = material_manager::get_from_path(component.material_inst_path);
						} else
							LOG(Warn, "Tryed to drop a material-instance asset but provided asset")
					}
					ImGui::EndDragDropTarget();
				}
			});
		});

	}


	void try_display_relationship_comp(PFF::entity entity) {			// , const std::unordered_map<UUID, entt::entity>& m_entity_map

		UI::try_display_component<relationship_component>("relationships", entity, [&](auto& component) {

			if (component.parent_ID == 0)
				UI::table_row("parent entity", (std::string_view)"no parent set");
			else
				UI::table_row_text("parent entity", "%s", entity.get_name().c_str());


			UI::table_row([]() { ImGui::Text("child entitys"); }, [&]() {

				if (component.children_ID.empty()) {
					ImGui::Text("no children");
				} else {
					
					for (const auto child_ID : component.children_ID) {
						
						const std::string child_name = application::get().get_world_layer()->get_map()->get_entity_by_UUID(child_ID).get_name();
						ImGui::Text("%s", child_name.c_str());
					}
				}
			});
		});

	}


	void try_display_procedural_script_comp(PFF::entity entity) {

		if (!script_system::is_ready())
			return;

		UI::try_display_component<procedural_mesh_component>("Mesh", entity, [](auto& component) {

			UI::table_row([]() {

				ImGui::Text("source");

			}, [&]() {

				u32 count = 0;
				static const char** items = script_system::get_all_procedural_mesh_scripts(&count);
				static int item_current_idx = static_cast<std::underlying_type_t<mobility>>(component.mobility_data);
				const char* combo_preview_value = items[item_current_idx];
				static ImGuiComboFlags flags = 0;
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
				if (ImGui::BeginCombo("##details_window_procedural_mesh_component_source", combo_preview_value, flags)) {

					for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
						const bool is_selected = (item_current_idx == n);
						if (ImGui::Selectable(items[n], is_selected)) {

							item_current_idx = n;

							// TODO: remove old script and then add new script
						}

						if (is_selected)		// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

			});
			UI::table_row([]() {

				ImGui::Text("mobility");

			}, [&]() {

				static const char* items[] = { "locked", "movable", "dynamic" };
				static int item_current_idx = static_cast<std::underlying_type_t<mobility>>(component.mobility_data);
				const char* combo_preview_value = items[item_current_idx];
				static ImGuiComboFlags flags = 0;
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
				if (ImGui::BeginCombo("##details_window_procedural_mesh_component_mobility", combo_preview_value, flags)) {

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
			UI::table_row("shoudl render", component.shoudl_render);

			UI::end_table();

			script_system::display_properties(component.script_name, (PFF::entity_script*)component.instance);

		}, false);

	}


	void display_name_comp(PFF::entity entity) {

		UI::try_display_component<name_component>("name", entity, [](auto& component) {

			static bool enable_name_editing = false;
			UI::table_row("name", component.name, enable_name_editing);
		});
	}


	void display_transform_comp(PFF::entity entity, const bool display_rotation_in_degree) {

		UI::try_display_component<transform_component>("Transform", entity, [&](auto& component) {

			auto& entity_transform = (glm::mat4&)component;
			glm::mat4 buffer_transform = entity_transform;

			if (UI::table_row("transform", entity_transform, display_rotation_in_degree)) {

				const glm::mat4 root_transform = buffer_transform;
				buffer_transform = glm::inverse(buffer_transform) * entity_transform;		// trandform delta
				entity.propegate_transform_to_children(root_transform, buffer_transform);
			}

		});
	}

}
