#pragma once

#include "util/pffpch.h"

#include <entt/entt.hpp>

#include "util/data_structures/UUID.h"
#include "map.h"
#include "components.h"


namespace PFF {

	class map;


	// template<typename T>
	// T find_owner_of_component(const entt::registry &reg, const T& component) {
    //     auto view = reg.view<T>();
    //     for (auto [entity, loc_component] : view.each())
    //         if (loc_component == component)
    //             return entity;

    //     return entt::null; // Return entt::null if no entity is found
    // };


	class entity {
	public:

		friend class map;

		template<typename T>
		bool has_component() { return m_map->m_registry.view<T>().contains(m_entity_handle); }

		PFF_DEFAULT_CONSTRUCTORS(entity);
		entity(entt::entity handle, map* map)
			: m_entity_handle(handle), m_map(map) { }

		template<typename T, typename... Args>
		FORCEINLINE T& add_component(Args&&... args) {

			ASSERT(!this->has_component<T>(), "", "Entity already has component!");
			T& component = m_map->m_registry.emplace<T>(m_entity_handle, std::forward<Args>(args)...);

			if (std::is_same_v(T, procedural_mesh_component)) {¬
				
				LOG(Fatal, "ADDING PROC MESH")
			}

			// m_map->on_component_added<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		FORCEINLINE T& add_or_replace_component(Args&&... args) { 

			T& component = m_map->m_registry.emplace_or_replace<T>(m_entity_handle, std::forward<Args>(args)...);
			// m_map->on_component_added<T>(*this, component);
			return component;
		}

		template<typename T>
		FORCEINLINE T& get_component() {

			ASSERT(this->has_component<T>(), "", "Entity does not have component!");
			return m_map->m_registry.get<T>(m_entity_handle);
		}

		template<typename T>
		FORCEINLINE void remove_component() {

			ASSERT(this->has_component<T>(), "", "Entity does not have component!");
			m_map->m_registry.remove<T>(m_entity_handle);
		}

		template<typename T>
		FORCEINLINE void add_script_component() { add_component<script_component>().bind<T>(); }
		
		template<typename T>
		FORCEINLINE void add_procedural_mesh_component(std::string script_name) { LOG(Info, "Called to add a component"); add_component<procedural_mesh_component>().bind<T>(script_name); }

		bool is_valid();
		bool empty() { return m_entity_handle == entt::null; }
		void add_mesh_component(mesh_component& mesh_comp);
		void propegate_transform_to_children(const glm::mat4& root_transform, const glm::mat4& delta_transform);
		void accumulate_transform_from_parents(glm::mat4& transform);

		operator bool()							const { return (m_entity_handle != entt::null); }
		operator entt::entity()					const { return m_entity_handle; }
		operator u32()							const { return (u32)m_entity_handle; }
		bool operator==(const entity& other)	const { return m_entity_handle == other.m_entity_handle && m_map == other.m_map; }
		bool operator!=(const entity& other)	const { return !(*this == other); }
		bool operator<(const entity& other) 	const noexcept {
			if (m_map != other.m_map)
				return m_map < other.m_map;
			return m_entity_handle < other.m_entity_handle;
		}

		UUID get_UUID()							{ return get_component<ID_component>().ID; }
		const std::string& get_name()			{ return get_component<name_component>().name; }

	private:

		entt::entity		m_entity_handle{ entt::null };
		map*				m_map = nullptr;
	};

}
