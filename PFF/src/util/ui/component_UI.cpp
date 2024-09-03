
#include "util/pffpch.h"

#include "util/ui/pannel_collection.h"
#include "engine/resource_management/static_mesh_asset_manager.h"

#include "component_UI.h"

namespace PFF::UI {


	void try_display_mesh_comp(PFF::entity entity) {

		UI::try_display_component<mesh_component>("Mesh", entity, [](auto& component) {

			UI::table_row([]() {

				ImGui::Text("mobility");

			}, [&]() {

				static const char* items[] = { "locked", "movable", "dynamic" };
				static int item_current_idx = static_cast<std::underlying_type_t<mobility>>(component.mobility);
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
		});

	}

	PFF_API void display_tag_comp(PFF::entity entity) {
		
		UI::try_display_component<tag_component>("Tag", entity, [](auto& component) {

			static bool enable_tag_editing = false;
			UI::table_row("tag", component.tag, enable_tag_editing);
		});
	}

	PFF_API void display_transform_comp(PFF::entity entity) {

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
