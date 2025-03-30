#pragma once

#include <imgui.h>

#include "util/ui/pannel_collection.h"

#include "engine/world/components.h"
#include "engine/world/entity.h"

namespace PFF::UI {

	template<typename T, typename ui_function>
	FORCEINLINE void try_display_component(const char* name, PFF::entity entity, ui_function function, bool need_to_close_table = true) {

		if (!entity.has_component<T>())
			return;

		if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
			return;

		PFF::UI::begin_table("entity_component", false);

		auto& component = entity.get_component<T>();
		function(component);

		if (need_to_close_table)
			PFF::UI::end_table();
	}
	
	template<typename T, typename ui_function>
	FORCEINLINE void display_component(const char* name, PFF::entity entity, ui_function function) {

		if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
			return;

		PFF::UI::begin_table("entity_component", false);

		auto& component = entity.get_component<T>();
		function(component);

		PFF::UI::end_table();
	}

	void try_display_mesh_comp(PFF::entity entity);
	
	void try_display_procedural_script_comp(PFF::entity entity);

	void display_tag_comp(PFF::entity entity);
	
	void display_transform_comp(PFF::entity entity, const bool display_rotation_in_degree = true);

}
