
#include "util/pffpch.h"

#include "util/ui/pannel_collection.h"
#include "util/data_structures/path_manipulation.h"
#include "engine/resource_management/static_mesh_asset_manager.h"
#include "project/script_system.h"

#include "component_UI.h"

namespace PFF::UI {

	
	void try_display_mesh_comp(PFF::entity entity) {

		UI::try_display_component<mesh_component>("Mesh", entity, [](auto& component) {

			UI::table_row([]() {

				ImGui::Text("mesh asset");

			}, [&]() {

				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
				ImGui::Text(component.asset_path.string().c_str());

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_CONTENT_FILE")) {

						const std::filesystem::path file_path = (const char*)payload->Data;
						component.asset_path = util::extract_path_from_project_content_folder(file_path);
						component.mesh_asset = static_mesh_asset_manager::get_from_path(component.asset_path);
					}
					ImGui::EndDragDropTarget();
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

	void display_tag_comp(PFF::entity entity) {

		UI::try_display_component<tag_component>("Tag", entity, [](auto& component) {

			static bool enable_tag_editing = false;
			UI::table_row("tag", component.tag, enable_tag_editing);
		});
	}

	void display_transform_comp(PFF::entity entity) {

		UI::try_display_component<transform_component>("Transform", entity, [&](auto& component) {

			auto& entity_transform = (glm::mat4&)component;
			glm::mat4 buffer_transform = entity_transform;

			if (UI::table_row("transform", entity_transform)) {

				const glm::mat4 root_transform = buffer_transform;
				buffer_transform = glm::inverse(buffer_transform) * entity_transform;		// trandform delta
				entity.propegate_transform_to_children(root_transform, buffer_transform);
			}

		});
	}


}
