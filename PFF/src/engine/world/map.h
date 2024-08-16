#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>

namespace PFF {

	class entity;

	// @brief The map class represents a segment of the in-game world, functioning as a chunk in the world layer.
	// @brief A large in-game world can be split into different chunks, each represented by a map. The map can be as large as needed.
	// @brief Active maps are those that the engine should display, and they are managed within the world_layer, which contains multiple maps
	class PFF_API map {
	public:

		map();
		~map();

		// @brief Creates a copy of the given map.
		// @param [other] The map to copy.
		// @return A reference to the newly created copy of the map.
		static ref<map> copy(ref<map> other);
		
		// ---------------------------- entity ----------------------------

		PFF_DEFAULT_GETTER(entt::registry&, registry)

		// @brief Creates a new entity with an optional name.
		// @param name The name of the entity (default is an empty string).
		// @return The created entity.
		entity create_entity(const std::string& name = std::string());

		// @brief Creates a new entity with a specified UUID and an optional name.
		// @param [uuid] The UUID for the entity.
		// @param [name] The name of the entity (default is an empty string).
		// @return The created entity.
		entity create_entity_with_UUID(UUID uuid, const std::string& name = std::string());

		// @brief Destroys the specified entity.
		// @param [entity] The entity to destroy.
		void destroy_entity(entity entity);

		// @brief Duplicates the specified entity.
		// @param [entity] The entity to duplicate.
		// @return The duplicated entity.
		entity duplicate_entity(entity entity);

		// @brief Finds an entity by its name.
		// @param [name] The name of the entity to find.
		// @return The entity with the specified name, or an empty entity if not found.
		entity find_entity_by_name(std::string_view name);

		// @brief Gets an entity by its UUID.
		// @param [uuid] The UUID of the entity to get.
		// @return The entity with the specified UUID, or an empty entity if not found.
		entity get_entity_by_UUID(UUID uuid);

		// @brief Returns all entities in this map with the specified component(s).
		// @brief Example: get_all_entities_with<PFF::ID_component>();
		// @tparam [components] The component types to filter entities by.
		// @return A view of all entities with the specified component(s).
		template<typename... Components>
		auto get_all_entities_with() { return m_registry.view<Components...>(); }

		// ---------------------------- runtime/simulation ----------------------------

		FORCEINLINE bool is_active() const { return m_system_state == system_state::active; }
		FORCEINLINE bool is_hidden() const { return m_system_state == system_state::suspended; }

		// @brief Pauses or unpauses the system.
		// @param [should_pause] Whether the system should be paused.
		void pause(bool should_pause);

		void on_runtime_start();
		void on_runtime_stop();
		void on_simulation_start();
		void on_simulation_stop();

		void on_update(const f32 delta_time);

		void serialize(serializer::option option);

	private:

		template<typename T>
		FORCEINLINE void on_component_added(entity entity, T& component) {

			if constexpr (std::is_same_v<T, mesh_component>) {

				CORE_LOG(Trace, "USING THE [on_component_added<mesh_component>()] template");

			}
		}

		friend class entity;
		friend class world_viewport_window;

		glm::mat4								m_transform;					// transform of map (map could be a small chunk of the ingame world)
		f64										m_width, m_length, m_height;	// size of map
		entt::registry							m_registry;
		std::unordered_map<UUID, entt::entity>	m_entity_map;
		system_state							m_system_state = system_state::active;
		std::filesystem::path					m_path;
	};

}
