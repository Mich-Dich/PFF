
#include "util/pffpch.h"

#include <entt.hpp>

#include "map.h"
#include "components.h"
#include "util/UUID.h"

namespace PFF {

	class entity {
	public:

		PFF_DEFAULT_CONSTRUCTORS(entity);
		entity(entt::entity handle, map* map)
			: m_entity_handle(handle), m_map(map) { }

		template<typename T, typename... Args>
		T& add_component(Args&&... args) {

			CORE_ASSERT(!has_component<T>(), "", "Entity already has component!");
			T& component = m_map->m_registry.emplace<T>(m_entity_handle, std::forward<Args>(args)...);
			//m_map->on_component_added<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& add_or_replace_component(Args&&... args) {
			
			T& component = m_map->m_registry.emplace_or_replace<T>(m_entity_handle, std::forward<Args>(args)...);
			//m_map->on_component_added<T>(*this, component);
			return component;
		}

		template<typename T>
		T& get_component() {

			CORE_ASSERT(has_component<T>(), "", "Entity does not have component!");
			return m_map->m_registry.get<T>(m_entity_handle);
		}

		template<typename T>
		void remove_component() {

			CORE_ASSERT(has_component<T>(), "", "Entity does not have component!");
			m_map->m_registry.remove<T>(m_entity_handle);
		}

		template<typename T>
		bool has_component() { return m_map->m_registry.view<T>().contains(m_entity_handle); }

		operator bool()							const { return (m_entity_handle != entt::null); }
		operator entt::entity()					const { return m_entity_handle; }
		operator u32()							const { return (u32)m_entity_handle; }
		bool operator==(const entity& other)	const { return m_entity_handle == other.m_entity_handle && m_map == other.m_map; }
		bool operator!=(const entity& other)	const { return !(*this == other); }

		UUID get_UUID()							{ return get_component<ID_component>().ID; }
		const std::string& get_name()			{ return get_component<tag_component>().tag; }

	private:

		entt::entity		m_entity_handle{ entt::null };
		map*				m_map = nullptr;
	};

}
