#pragma once

#include "util/pffpch.h"
#include "engine/render/render_public_data.h"
#include "engine/render/material.h"
#include "util/math/math.h"
#include "engine/geometry/mesh.h"

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace PFF {

	struct transform_component {

		PFF_DEFAULT_CONSTRUCTORS(transform_component);
		
		transform_component(const glm::vec3& translation)
			: translation(translation) {}
		transform_component(const glm::mat4& transform) { math::decompose_transform(transform, translation, rotation, scale); }

		operator glm::mat4 () { return get_transform(); }
		operator const glm::mat4 () const { return get_transform(); }

		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale	= { 1.0f, 1.0f, 1.0f };

	//private:

		FORCEINLINE glm::mat4 get_transform() const {

			return glm::translate(glm::mat4(1.0f), translation)
				* glm::toMat4(glm::quat(rotation))
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct ID_component {

		PFF_DEFAULT_CONSTRUCTORS(ID_component);

		UUID ID;
	};

	// Tag component is a small string often used to find entities that have the same tag
	struct tag_component {
		
		PFF_DEFAULT_CONSTRUCTORS(tag_component);
		tag_component(const std::string& tag)
			: tag(tag) {}
		
		std::string tag;
	};
	
	// ------------------------------------------------------------------------------------------------------------------------
	// MESH FOR RENDERER		=> need this component and the transform component
	// ------------------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------------------
	// TODO: Move this to a render_resourse_manager that automaticly uploads/removes meshes/materials to GPU

//	struct mesh{
//		u32					index_count;
//		u32					first_index;
//		glm::mat4			transform_offset;		// offset from the parent => can build chains
//#if defined PFF_RENDER_API_VULKAN
//		VkBuffer			index_buffer;
//		VkDeviceAddress		vertex_bffer_address;
//#endif
//		material_instance*	material;
//	};
// ----------------------------------------------------------------------------------------------------------------------------


	struct mesh_component {

		PFF_DEFAULT_CONSTRUCTORS(mesh_component);

		mesh_component(ref<PFF::geometry::mesh_asset> mesh_asset_ref)
			: mesh_asset(mesh_asset_ref) {}

		glm::mat4							transform = glm::mat4(1);
		bool								shoudl_render = true;
		ref<PFF::geometry::mesh_asset>		mesh_asset;
		material_instance*					material;					// TODO: currently uses one material for all surfaces in mesh_asset, change so it can use diffrent materials
	};



	template<typename... component>
	struct component_group { };

	using all_components =
		component_group<
			transform_component, ID_component, tag_component, mesh_component
		>;

}
