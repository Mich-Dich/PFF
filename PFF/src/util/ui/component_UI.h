#pragma once

#include <imgui.h>
#include "engine/world/components.h"
#include "engine/world/entity.h"

namespace PFF::UI {

	template<typename T, typename ui_function>
	FORCEINLINE void try_display_component(const char* name, PFF::entity entity, ui_function function, bool need_to_close_table = true) {

		if (!entity.has_component<T>())
			return;

		if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
			return;

		UI::begin_table("entity_component", false);

		auto& component = entity.get_component<T>();
		function(component);

		if (need_to_close_table)
			UI::end_table();
	}
	
	template<typename T, typename ui_function>
	FORCEINLINE void display_component(const char* name, PFF::entity entity, ui_function function) {

		if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
			return;

		UI::begin_table("entity_component", false);

		auto& component = entity.get_component<T>();
		function(component);

		UI::end_table();
	}

	FORCEINLINE PFF_API void try_display_mesh_comp(PFF::entity entity);
	FORCEINLINE PFF_API void try_display_procedural_script_comp(PFF::entity entity);

	FORCEINLINE PFF_API void display_tag_comp(PFF::entity entity);
	FORCEINLINE PFF_API void display_transform_comp(PFF::entity entity);

}
