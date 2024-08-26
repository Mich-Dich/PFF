#pragma once

#include "util/pffpch.h"
#include "engine/render/render_public_data.h"
#include "engine/render/material.h"
#include "util/math/math.h"
#include "engine/geometry/mesh.h"


//#include <xmmintrin.h>   // SSE
//#include <smmintrin.h>   // SSE4.1 (for matrix multiplication)

namespace PFF {

	enum class transform_operation {

		none,
		translate = 7,		// 7 is from the ImGuizmo lib
		rotate = 120,		// 120 is from the ImGuizmo lib
		scale = 896,		// 896 is from the ImGuizmo lib
	};

	struct transform_component {

		PFF_DEFAULT_CONSTRUCTORS(transform_component);
		
		transform_component(const glm::mat4& transform)
			: transform(transform) {}

		operator glm::mat4& () { return transform; }
		operator const glm::mat4& () { return transform; }

		glm::mat4 transform = glm::mat4(1);
	};

	struct ID_component {

		PFF_DEFAULT_CONSTRUCTORS(ID_component);

		UUID								ID;
	};

	/* MAYBE
		// every entity has a unique name
		struct name_component {

			PFF_DEFAULT_CONSTRUCTORS(name_component);
			name_component(const std::string& name)
				: name(name) {}

			std::string							name;
		};
	*/

	// Tag component is a small string often used to find entities that have the same tag (eg: teammates, bullets, ...)
	struct tag_component {

		PFF_DEFAULT_CONSTRUCTORS(tag_component);
		tag_component(const std::string& tag)
			: tag(tag) {}

		std::string							tag;
	};

	// ------------------------------------------------------------------------------------------------------------------------
	// MESH FOR RENDERER		=> need this component and the transform component
	// ------------------------------------------------------------------------------------------------------------------------

	// MAYBE: move to file private_components => this file should not be exposed to the user
	struct mesh_component {
	public:

		PFF_DEFAULT_CONSTRUCTORS(mesh_component);

		mesh_component(ref<PFF::geometry::mesh_asset> mesh_asset_ref)
			: mesh_asset(mesh_asset_ref) {}

		mobility							mobility = mobility::locked;
		bool								shoudl_render = true;
		std::filesystem::path				asset_path;							// MAYBE: doesnt need to be saved here, only needed for serializaation, maybe export to asset manager or UUID

	// MAYBE: make private
		ref<PFF::geometry::mesh_asset>		mesh_asset = nullptr;
		material_instance*					material = nullptr;					// TODO: currently uses one material for all surfaces in mesh_asset, change so it can use diffrent materials per furface
	};

	// ============================================================ IN-DEV ============================================================

	struct static_mesh_component {

		PFF_DEFAULT_CONSTRUCTORS(static_mesh_component);

		glm::mat4							transform = glm::mat4(1);
		ref<PFF::geometry::mesh_asset>		mesh_asset;
		material_instance*					material = nullptr;					// TODO: currently uses one material for all surfaces in mesh_asset, change so it can use diffrent materials
	};

	// can be used only when needed OR every entity has it		(leaning towarts sparing use)
	struct relationship_component {

		PFF_DEFAULT_CONSTRUCTORS(relationship_component);
		relationship_component(const UUID parent_ID)
			: parent_ID(parent_ID) {}
		relationship_component(const UUID parent_ID, const std::vector<UUID> children_ID)
			: parent_ID(parent_ID), children_ID(children_ID) {}

		UUID								parent_ID;
		std::vector<UUID>					children_ID;
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

	struct procedural_mesh_component {

		template<typename T>
		void bind() {

			create_script = []() { return static_cast<entity_script*>(new T()); };
			destroy_script = [](script_component* script_comp) { delete script_comp->instance; script_comp->instance = nullptr; };
		}

		entity_script* instance = nullptr;
		entity_script* (*create_script)();
		void			(*destroy_script)(script_component*);
	
	};

	// ============================================================ IN-DEV ============================================================

	template<typename... component>
	struct component_group { };

	using all_components =
		component_group<
			transform_component, ID_component, tag_component, mesh_component, static_mesh_component, script_component //, procedural_mesh_component
		>;

}
