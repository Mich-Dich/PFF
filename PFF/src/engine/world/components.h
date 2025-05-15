#pragma once

#include "util/pffpch.h"

#include "util/math/math.h"
#include "util/data_structures/UUID.h"
#include "engine/render/render_public_data.h"
#include "engine/render/material.h"
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

		glm::mat4 									transform = glm::mat4(1);
	};

	struct ID_component {

		PFF_DEFAULT_CONSTRUCTORS(ID_component);
		ID_component(const UUID& uuid)
			: ID(uuid) {}

		UUID										ID{};
	};

	// Tag component is a small string often used to find entities that have the same tag (eg: teammates, bullets, ...)
	struct name_component {

		PFF_DEFAULT_CONSTRUCTORS(name_component);
		name_component(const std::string& name)
			: name(name) {}

		std::string									name{};
	};

	// MAYBE
	// 	every entity has a unique name
	// 	struct tag_component {

	// 		PFF_DEFAULT_CONSTRUCTORS(name_component);
	// 		name_component(const std::string& name)
	// 			: name(name) {}

	// 		std::string								tags;
	// 	};
	
	struct enum_tag_component {

		PFF_DEFAULT_CONSTRUCTORS(enum_tag_component);
		enum_tag_component(const u64 value)
			: value(value) {}

		u64											value = 0;					// cast this to the actual emun you want to use
	};

	struct flag_tag_component {

		PFF_DEFAULT_CONSTRUCTORS(flag_tag_component);
		flag_tag_component(const u64 flags)
			: flags(flags) {}

		u64											flags = 0;					// cast this to the actual emun you want to use
	};

	// TODO: iterate over all lifetime_comps in world.update()
	struct simple_lifetime_component {

		PFF_DEFAULT_CONSTRUCTORS(simple_lifetime_component);
		simple_lifetime_component(f32 total_lifetime, f32 passed_time, bool auto_destroy)
			: total_lifetime(total_lifetime), passed_time(passed_time), auto_destroy(auto_destroy) {}

		f32											total_lifetime = 0;			// target lifetime
		f32											passed_time = 0;			// time the component is already alive
		bool    									auto_destroy = true;		// wheter to automaticly destroy entity when time expires
	};

	// TODO: iterate over all lifetime_comps in world.update()
	struct lifetime_component {

		PFF_DEFAULT_CONSTRUCTORS(lifetime_component);
		lifetime_component(f32 total_lifetime, f32 passed_time, bool auto_destroy, std::function<void()> on_expired, std::function<void(f32)> on_update)
			: total_lifetime(total_lifetime), passed_time(passed_time), auto_destroy(auto_destroy), on_expired(on_expired), on_update(on_update) {}

		f32 										total_lifetime = 0;			// target lifetime
		f32 										passed_time = 0;			// time the component is already alive
		bool 										auto_destroy = true;		// wheter to automaticly destroy entity when time expires
		bool 										is_paused = false;			// For pausing/resuming lifetime countdown

		// Optional callback functionality		
		std::function<void()> 						on_expired;
		std::function<void(f32)> 					on_update; 					// float is progress (0-1)
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

		mobility									mobility_data = mobility::locked;
		bool										shoudl_render = true;
		ref<PFF::geometry::mesh_asset>				mesh_asset = nullptr;				// MAYBE: make private
		std::filesystem::path						asset_path;							// MAYBE: doesnt need to be saved here, only needed for serializaation, maybe export to asset manager or UUID
		ref<material_instance>						material = nullptr;					// TODO: currently uses one material for all surfaces in mesh_asset, change so it can use diffrent materials per furface
		std::filesystem::path						material_inst_path;					// MAYBE: doesnt need to be saved here, only needed for serializaation, maybe export to asset manager or UUID
	};

	//struct static_mesh_component {

	//	PFF_DEFAULT_CONSTRUCTORS(static_mesh_component);

	//	glm::mat4							transform = glm::mat4(1);
	//	ref<PFF::geometry::mesh_asset>		mesh_asset;
	//	material_instance*					material = nullptr;					// TODO: currently uses one material for all surfaces in mesh_asset, change so it can use diffrent materials
	//};

	// can be used only when needed OR every entity has it		(leaning towarts sparing use)
	struct relationship_component {

		PFF_DEFAULT_CONSTRUCTORS(relationship_component);
		relationship_component(const UUID parent_ID)
			: parent_ID(parent_ID) {}
		relationship_component(const UUID parent_ID, const std::vector<UUID> children_ID)
			: parent_ID(parent_ID), children_ID(children_ID) {}

		UUID										parent_ID;
		std::vector<UUID>							children_ID;
	};

	// ============================================================ IN-DEV ============================================================

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

		entity_script*								instance = nullptr;
		entity_script*								(*create_script)();
		void										(*destroy_script)(script_component*);

		friend class map;
		friend class entity;

		// I chose to use inheritence (virtual functions) because the marginal cost of v-tables is much less than of a heavy component
		// the version with virtual function had a size of around [328 bytes] now its [24 bytes]
	};

	class procedural_mesh_script;
	struct procedural_mesh_component {
		
		PFF_DEFAULT_CONSTRUCTORS(procedural_mesh_component);

		// --------------------- script-part of component --------------------- 
		template<typename T>
		void bind(std::string& script_name_string) {

			create_script = []() { return static_cast<procedural_mesh_script*>(new T()); };
			destroy_script = [](procedural_mesh_component* script_comp) { delete script_comp->instance; script_comp->instance = nullptr; };
			script_name = script_name_string;
		}

		procedural_mesh_script*						instance = nullptr;
		procedural_mesh_script*						(*create_script)();
		void										(*destroy_script)(procedural_mesh_component*);
		std::string									script_name = "";		// MAYBE: doesnt need to be saved here, only needed for serialization, maybe export to asset manager or UUID

		// --------------------- mesh-part of component --------------------- 
		mobility									mobility_data = mobility::locked;
		bool										shoudl_render = true;
		//ref<PFF::geometry::mesh_asset>			mesh_asset = nullptr;	// MAYBE: Move mesh_asset to the component for better performance in rendering
		//material_instance*						material = nullptr;		// TODO: currently uses one material for all surfaces in mesh_asset, change so it can use diffrent materials per furface
	};

	// ============================================================ IN-DEV ============================================================

	template<typename... component>
	struct component_group { };

	using all_components =
		component_group<
			transform_component, ID_component, name_component,
			mesh_component, script_component, procedural_mesh_component,
			enum_tag_component, flag_tag_component,
			simple_lifetime_component, lifetime_component
		>;

}
