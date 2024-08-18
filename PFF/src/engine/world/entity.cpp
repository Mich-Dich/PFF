
#include "util/pffpch.h"

#include "application.h"
#include "engine/resource_management/static_mesh_asset_manager.h"
#include "engine/resource_management/mesh_serializer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> // For glm::translate, glm::rotate
#include <glm/gtx/euler_angles.hpp> // For glm::eulerAngleYXZ

#include "entity.h"

namespace PFF {

	void entity::add_mesh_component(mesh_component& mesh_comp) {

		CORE_ASSERT(!mesh_comp.asset_path.empty(), "", "Provided path is empty");

		asset_file_header asset_header;
		general_mesh_file_header general_header;
		auto serializer = serializer::binary(application::get().get_project_path() / CONTENT_DIR / mesh_comp.asset_path, "PFF_asset_file", serializer::option::load_from_file);
		serialize_mesh_headers(serializer, asset_header, general_header);
		CORE_ASSERT(asset_header.type == file_type::mesh, "", "Tryed to add mesh_component but provided asset_path is not a mesh");

		switch (general_header.type) {
		case mesh_type::static_mesh:
		{

			static_mesh_file_header static_mesh_header;
			serialize_static_mesh_header(serializer, static_mesh_header);

			mesh_comp.mesh_asset = static_mesh_asset_manager::get_from_path(mesh_comp.asset_path);

			//TODO: check 
			//		if (user defined a material_instance) || ([static_mesh_header] has a material_instance)
			//		else use default_material
			CORE_LOG(Warn, "Added mesh comp <= STILL NEED TO CHECK FOR DEFAULT MATERIAL");

		} break;

		case mesh_type::dynamic_mesh:
		case mesh_type::skeletal_mesh:
		case mesh_type::mesh_collection:
		case mesh_type::procedural_mesh:
			CORE_LOG(Trace, "Adding of mesh_type not supported yet"); break;

		default: CORE_LOG(Trace, "unidentified [mesh_type] used"); break;
		}


		// ---------------------------------- does not have mesh component  => add component to root ----------------------------------
		if (!has_component<mesh_component>()) {		

			add_component<mesh_component>(mesh_comp);
			return;
		}

		// ---------------------------------- already has mesh_component  => add new entity and make child of this entity ----------------------------------
		entity child = m_map->create_entity("SM_" + mesh_comp.asset_path.filename().replace_extension("").string());
		child.add_component<relationship_component>(get_component<ID_component>().ID);		// register child
		child.add_component<mesh_component>(mesh_comp);

		// ---------------------------------- current entity already has relationship ----------------------------------
		if (has_component<relationship_component>()) {
			
			get_component<relationship_component>().children_ID.push_back(child.get_component<ID_component>().ID);
			return;
		}

		std::vector<UUID> cildren{};
		cildren.push_back(child.get_component<ID_component>().ID);
		add_component<relationship_component>(NULL, cildren);		// register child
		return;

	}
	
#if 0

	void entity::propegate_transform_to_children(const glm::mat4& transform, const transform_operation transform_operation) {

		if (!has_component<relationship_component>())		// is standalone entity => no relationships
			return;

		auto& relation_comp = get_component<relationship_component>();
		if (relation_comp.children_ID.size() <= 0)			// has no children
			return;


		for (const auto child_ID : relation_comp.children_ID) {

			// Apply the transform to the child
			auto child = m_map->get_entity_by_UUID(child_ID);
			glm::mat4& child_transform = (glm::mat4&)child.get_component<transform_component>();
			child_transform = new_transform * child_transform;

			child.propegate_transform_to_children(new_transform, transform_operation);			// call recursive for children
		}

	}

#else

	void entity::propegate_transform_to_children(const glm::mat4& transform, const transform_operation transform_operation) {

		static u32 recursion_count = 0;

		if (!has_component<relationship_component>())		// is standalone entity => no relationships
			return;

		auto& relation_comp = get_component<relationship_component>();
		if (relation_comp.children_ID.size() <= 0)			// has no children
			return;

		recursion_count++;
		//CORE_LOG(Debug, "PUSH recursion: " << recursion_count);

		glm::vec3 parent_translation, parent_rotation, parent_scale;
		math::decompose_transform((glm::mat4&)get_component<transform_component>(), parent_translation, parent_rotation, parent_scale);

		glm::vec3 transform_translation, transform_rotation, transform_scale;
		math::decompose_transform(transform, transform_translation, transform_rotation, transform_scale);


		glm::mat4 new_transform;

		switch (transform_operation) {

		case transform_operation::translate:		// recalc transform for every recursion
			transform_translation *= parent_scale;
			math::compose_transform(new_transform, transform_translation, transform_rotation, transform_scale);
			break;

		case transform_operation::rotate:			// use original transform and hand that down every recursion

			if (recursion_count == 1) {				// Is root

				// This works for the first generation of children, but only that generation
				glm::mat4 translate_to_parent = glm::translate(glm::mat4(1.0f), parent_translation);
				glm::mat4 rotation_matrix = glm::yawPitchRoll(transform_rotation.y, transform_rotation.x, transform_rotation.z);
				glm::mat4 translate_back = glm::translate(glm::mat4(1.0f), -parent_translation);
				new_transform = translate_to_parent * rotation_matrix * translate_back;
			} else
				new_transform = transform;

			break;

		case transform_operation::scale:
			break;

		default:
			break;
		}

		for (const auto child_ID : relation_comp.children_ID) {

			// Apply the transform to the child
			auto child = m_map->get_entity_by_UUID(child_ID);
			glm::mat4& child_transform = (glm::mat4&)child.get_component<transform_component>();
			child_transform = new_transform * child_transform;

			child.propegate_transform_to_children(new_transform, transform_operation);			// call recursive for children
		}

		recursion_count--;
		//CORE_LOG(Debug, "POP  recursion: " << recursion_count);
	}

#endif

	void entity::accumulate_transform_from_parents(glm::mat4& transform) {

		CORE_LOG(Warn, "Not implemented yet");
	}

}
