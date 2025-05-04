#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "util/data_structures/UUID.h"
// #include "engine/world/entity.h"
#include "components.h"

namespace PFF {

	// TODO: move to world.h when implemented
	struct dynamic_skybox_data {

		glm::mat4				inverse_view{};
		glm::vec4				middle_sky_color = glm::vec4(0.f, 0.12f, 0.26f, 1.f);
		glm::vec4				horizon_sky_color = glm::vec4(0.19f, 0.39f, 0.52f, 1.f);
		glm::vec4				sunset_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.f);
		glm::vec4				night_color = glm::vec4(0.05f, 0.05f, 0.1f, 1.f);
		glm::vec2				image_size{};
		f32 					sun_distance = 1000000.f;
		f32 					sun_radius = 565.f;																// --- allignment
    	f32						sun_core_falloff = .99f;          	// [0.8-0.99] smaller = softer core
    	f32						sun_glow_radius_multiplier = 25.5f; // [1.5-4.0] glow halo size
    	f32						sun_glow_intensity = .57f;        	// [0.1-1.0] glow brightness
    	f32						sun_glow_falloff = 5.f;          	// [1.0-4.0] glow edge hardness

		f32 					FOV_y = 45.f;

    	// Cloud parameters
		f32 					cloud_height = 100.f;
		f32 					time = 0.f;
		f32 					cloud_density = 0.5f;																// --- allignment
		glm::vec4 				cloud_color = glm::vec4(0.95f, 0.95f, 1.0f, 0.5f);
		glm::vec4 				cloud_color_night = glm::vec4(0.0f, 0.0f, 0.0f, 0.9f);
		glm::vec2 				cloud_speed = glm::vec2(0.1f, 0.2f);
		f32 					cloud_scale = 100.f;
		f32 					cloud_coverage = .6f;
		f32						cloud_persistence = .6f;
		f32						cloud_detail = 4.6f;
		u32						cloud_octaves = 3;																	// --- allignment
	};
	
	class entity;

	// @brief The map class represents a segment of the in-game world, functioning as a chunk in the world layer.
	// @brief A large in-game world can be split into different chunks, each represented by a map. The map can be as large as needed.
	// @brief Active maps are those that the engine should display, and they are managed within the world_layer, which contains multiple maps
	class map {
	public:

		map();
		~map();

		// @brief Creates a copy of the given map.
		// @param [other] The map to copy.
		// @return A reference to the newly created copy of the map.
		static ref<map> copy(ref<map> other);
		
		// ---------------------------- entity ----------------------------

		PFF_DEFAULT_GETTER(entt::registry&, 				registry)
		PFF_DEFAULT_GETTER_REF(PFF::dynamic_skybox_data, 	dynamic_skybox_data)

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
		void recreate_scripts();

		void serialize(PFF::serializer::option option);


	private:

		friend class entity;
		friend class world_viewport_window;

		// template<typename T>
		// FORCEINLINE void on_component_added(entity map_entity, T& component) {

		// 	if constexpr (std::is_same_v<T, mesh_component>) {

		// 	}
		// }

		glm::mat4								m_transform;					// transform of map (map could be a small chunk of the ingame world)
		f64										m_width, m_length, m_height;	// size of map
		entt::registry							m_registry;
		std::unordered_map<UUID, entt::entity>	m_entity_map;
		system_state							m_system_state = system_state::active;
		std::filesystem::path					m_path;
		dynamic_skybox_data						m_dynamic_skybox_data{};
	};

}
