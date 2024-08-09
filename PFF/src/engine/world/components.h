#pragma once

#include "util/pffpch.h"
#include "engine/render/render_public_data.h"
#include "engine/render/material.h"
#include "util/math/math.h"
#include "engine/geometry/mesh.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <xmmintrin.h>   // SSE
#include <smmintrin.h>   // SSE4.1 (for matrix multiplication)

namespace PFF {

	struct transform_component {

		PFF_DEFAULT_CONSTRUCTORS(transform_component);
		
		transform_component(const glm::vec3& translation)
			: translation(translation) {}
		transform_component(const glm::mat4& transform) { math::decompose_transform(transform, translation, rotation, scale); }

		operator glm::mat4& () { return get_transform(); }
		operator const glm::mat4& () { return get_transform(); }

		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale	= { 1.0f, 1.0f, 1.0f };

	private:

		bool needs_updating = true;
		glm::mat4 transform = glm::mat4(1);

		FORCEINLINE glm::mat4& get_transform() {

			if (needs_updating) {
				needs_updating = false;

				glm::mat4 translation_matrix = glm::mat4(1.0f);
				glm::mat4 scale_matrix = glm::mat4(1.0f);
				glm::mat4 rotation_matrix = glm::mat4(1.0f);

				translation_matrix[3] = glm::vec4(translation.x, translation.y, translation.z, 1.0f);
				
				scale_matrix[0][0] = scale.x;
				scale_matrix[1][1] = scale.y;
				scale_matrix[2][2] = scale.z;

				glm::quat quaternion(rotation);
				rotation_matrix = glm::toMat4(quaternion);

				// Combine matrices using SIMD
				transform = translation_matrix * rotation_matrix * scale_matrix;
			}

			return transform;
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

		mobility							mobility = mobility::locked;
		glm::mat4							transform = glm::mat4(1);
		bool								shoudl_render = true;
		ref<PFF::geometry::mesh_asset>		mesh_asset;
		material_instance*					material = nullptr;					// TODO: currently uses one material for all surfaces in mesh_asset, change so it can use diffrent materials
	};

	// ============================================================ IN-DEV ============================================================

	struct static_mesh_component {

		PFF_DEFAULT_CONSTRUCTORS(static_mesh_component);

		glm::mat4							transform = glm::mat4(1);
		ref<PFF::geometry::mesh_asset>		mesh_asset;
		material_instance*					material = nullptr;					// TODO: currently uses one material for all surfaces in mesh_asset, change so it can use diffrent materials
	};

	class entity_script;

	// @brief This component should not be used directly, but through the API of entity
	// @brief		example: 
	// @brief		class test_script : public entity_script {
	// @brief		public:
	// @brief			void on_create() { LOG(Info, "Creating instance") }
	// @brief			void on_destroy() {}
	// @brief			void on_update(f32 delta_time) { LOG(Trace, "time: " << delta_time); }
	// @brief		};
	// @brief		entitiy.add_script_component<test_script>();
	struct script_component {
	private:

		template<typename T>
		void bind() {

			create_script = []() { return static_cast<entity_script*>(new T()); };
			destroy_script = [](script_component* script_comp) { delete script_comp->instance; script_comp->instance = nullptr; };
		}

		entity_script*	instance = nullptr;
		entity_script*	(*create_script)();
		void			(*destroy_script)(script_component*);

		friend class map;
		friend class entity;

		// I chose to use inheritence (virtual functions) because the marginal cost of v-tables is much less than of a heavy component
		// the version with virtual function had a size of around [328 bytes] now its [24 bytes]
	};

	// => EnTT hates empty structs!
	//struct procedural_mesh_component {
	//};

	// ============================================================ IN-DEV ============================================================

	template<typename... component>
	struct component_group { };

	using all_components =
		component_group<
			transform_component, ID_component, tag_component, mesh_component, static_mesh_component, script_component //, procedural_mesh_component
		>;

}
