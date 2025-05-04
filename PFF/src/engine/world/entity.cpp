
#include "util/pffpch.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> // For glm::translate, glm::rotate
#include <glm/gtx/euler_angles.hpp> // For glm::eulerAngleYXZ

#include "application.h"
#include "engine/resource_management/static_mesh_asset_manager.h"
#include "engine/resource_management/material/material_asset_manager.h"
#include "engine/resource_management/mesh_serializer.h"
// #include "map.h"

#include "entity.h"

namespace PFF {
	
	
	bool entity::is_valid() { return m_map->m_registry.valid(m_entity_handle); }


	void entity::add_mesh_component(mesh_component& mesh_comp) {

		ASSERT(!mesh_comp.asset_path.empty(), "", "Provided path is empty");

		asset_file_header asset_header;
		general_mesh_file_header general_header;
		auto serializer = serializer::binary(application::get().get_project_path() / CONTENT_DIR / mesh_comp.asset_path, "PFF_asset_file", serializer::option::load_from_file);
		serialize_mesh_headers(serializer, asset_header, general_header);
		ASSERT(asset_header.type == file_type::mesh, "", "Tryed to add mesh_component but provided asset_path is not a mesh");

		switch (general_header.type) {
			case mesh_type::static_mesh: {

				static_mesh_file_header static_mesh_header;
				serialize_static_mesh_header(serializer, static_mesh_header);
				mesh_comp.mesh_asset = static_mesh_asset_manager::get_from_path(mesh_comp.asset_path);

				if (!mesh_comp.material_inst_path.empty())
				mesh_comp.material = material_asset_manager::get_material_instance_from_path(mesh_comp.material_inst_path);

				//TODO: check 
				//		if (user defined a material_instance) || ([static_mesh_header] has a material_instance)
				//		else use default_material
				// LOG(Warn, "Added mesh comp <= STILL NEED TO CHECK FOR DEFAULT MATERIAL");

			} break;

			case mesh_type::dynamic_mesh:
			case mesh_type::skeletal_mesh:
			case mesh_type::mesh_collection:
			case mesh_type::procedural_mesh:
				LOG(Trace, "Adding of mesh_type not supported yet"); break;

			default: LOG(Trace, "unidentified [mesh_type] used"); break;
		}


		// ---------------------------------- does not have mesh component  => add component to root ----------------------------------
		if (!has_component<mesh_component>()) {		

			add_component<mesh_component>(mesh_comp);
			return;
		}

		// ---------------------------------- already has mesh_component  => add new entity and make child of this entity ----------------------------------
		const auto new_entity_name = mesh_comp.asset_path.filename().replace_extension("").string();
		entity child = m_map->create_entity("SM_" + new_entity_name);
		child.add_component<relationship_component>(get_component<ID_component>().ID);		// register child
		child.add_component<mesh_component>(mesh_comp);

		// ---------------------------------- current entity already has relationship ----------------------------------
		if (has_component<relationship_component>()) {
			
			get_component<relationship_component>().children_ID.push_back(child.get_component<ID_component>().ID);
			return;
		}

		// ---------------------------------- current entity doesnt have relationship => create new component ----------------------------------
		std::vector<UUID> cildren{};
		cildren.push_back(child.get_component<ID_component>().ID);
		add_component<relationship_component>( 0 /*get_UUID()*/, cildren);		// register child
		return;

	}
	
	void entity::propegate_transform_to_children(const glm::mat4& root_transform, const glm::mat4& delta_transform) {

		if (!has_component<relationship_component>())		// is standalone entity => no relationships
			return;

		auto& relation_comp = get_component<relationship_component>();
		if (relation_comp.children_ID.size() <= 0)			// has no children
			return;

		for (const auto child_ID : relation_comp.children_ID) {

			// Apply the transform to the child
			auto child = m_map->get_entity_by_UUID(child_ID);
			glm::mat4& child_transform = (glm::mat4&)child.get_component<transform_component>();
			glm::mat4 child_local_space_transform = glm::inverse(root_transform) * child_transform;
			child_local_space_transform = delta_transform * child_local_space_transform;		// Apply transform in local space
			child_transform = root_transform * child_local_space_transform;
			child.propegate_transform_to_children(root_transform, delta_transform);			// call recursive for children
		}
	}

	void entity::accumulate_transform_from_parents(glm::mat4& transform) {

		LOG(Warn, "Not implemented yet");
	}

}
