
#include "util/pffpch.h"

#include "components.h"
#include "engine/world/entity.h"

// !!!!!!!!!!!!!!!! DEV-ONLY !!!!!!!!!!!!!!!!!!!!!!
#include "engine/render/vulkan/vk_renderer.h"


#include "map.h"

namespace PFF {

	template<typename... component>
	static void copy_component(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap) {
		([&]() {
			auto view = src.view<component>();
			for (auto srcEntity : view) {
				entt::entity dstEntity = enttMap.at(src.get<ID_component>(srcEntity).ID);

				auto& srcComponent = src.get<component>(srcEntity);
				dst.emplace_or_replace<component>(dstEntity, srcComponent);
			}
			}(), ...);
	}

	template<typename... component>
	static void copy_component(component_group<component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& entt_map) {
		copy_component<component...>(dst, src, entt_map);
	}

	template<typename... component>
	static void copy_component_if_exists(entity dst, entity src) {
		([&]() {
			if (src.has_component<component>())
				dst.add_or_replace_component<component>(src.get_component<component>());
			}(), ...);
	}

	template<typename... component>
	static void copy_component_if_exists(component_group<component...>, entity dst, entity src) {
		copy_component_if_exists<component...>(dst, src);
	}

	// =======================================================================================================================================
	// - - - - - - - - MAP - - - - - - - - 
	// =======================================================================================================================================

	map::map() {


		entity loc_entitiy = create_entity("editor_origin_grid");

		auto& transform_comp = loc_entitiy.get_component<transform_component>();
		transform_comp.translation = glm::vec3(0);
		transform_comp.rotation = glm::vec3(0);

		auto& mesh_comp = loc_entitiy.add_component<mesh_component>();			
		mesh_comp.mesh_asset = GET_RENDERER.get_test_mesh();					// get correct mesh
		mesh_comp.material = GET_RENDERER.get_default_material_pointer();		// get correct shader


#define ADD_MESH_PROCESS 3
#if ADD_MESH_PROCESS == 0
		
		entity loc_entitiy = create_entity("Test entity for renderer");

		auto& transform_comp = loc_entitiy.get_component<transform_component>();
		transform_comp.translation = glm::vec3(0);

		auto& mesh_comp = loc_entitiy.add_component<mesh_component>();
		mesh_comp.mesh_asset = GET_RENDERER.get_test_mesh();
		mesh_comp.material = GET_RENDERER.get_default_material_pointer();

#elif ADD_MESH_PROCESS == 1

		for (u32 x = 0; x < 15; x++) {
			for (u32 y = 0; y < 10; y++) {

				entity loc_entitiy = create_entity("Test entity for renderer: " + util::to_string(x) + " / " + util::to_string(y));

				auto& transform_comp = loc_entitiy.get_component<transform_component>();
				transform_comp.translation = glm::vec3(35 * x, 0, 75 * y);
				transform_comp.scale = glm::vec3(0.1f);

				auto& mesh_comp = loc_entitiy.add_component<mesh_component>();
				mesh_comp.mesh_asset = GET_RENDERER.get_test_mesh();
				mesh_comp.material = GET_RENDERER.get_default_material_pointer();

				// ============== ONLY FOR STATIC OBJECTS ==============
				mesh_comp.transform = (glm::mat4&)transform_comp * mesh_comp.transform;
			}
		}

#elif ADD_MESH_PROCESS == 2

		const f32 field_size = 20.f;
		util::random loc_random{};

		for (u32 x = 0; x < 2000; x++) {

			entity loc_entitiy = create_entity("Test entity for renderer: " + util::to_string(x));

			auto& transform_comp = loc_entitiy.get_component<transform_component>();
			transform_comp.translation = glm::vec3(loc_random.get_f32(-field_size, field_size), 0, loc_random.get_f32(-field_size, field_size));
			transform_comp.rotation = glm::vec3(0, loc_random.get_f32(0.f, 2 * glm::pi<f32>()), 0);

			auto& mesh_comp = loc_entitiy.add_component<mesh_component>();
			mesh_comp.mesh_asset = GET_RENDERER.get_test_mesh();
			mesh_comp.material = GET_RENDERER.get_default_material_pointer();

			// ============== ONLY FOR STATIC OBJECTS ==============
			mesh_comp.transform = (glm::mat4&)transform_comp * mesh_comp.transform;
		}

#endif //  ADD_MESH_PROCESS == 1
		
	}

	map::~map() { }

	ref<map> map::copy(ref<map> other) { return ref<map>(); }

	// =============================================================== entity ===============================================================

	entity map::create_entity(const std::string& name) { return create_entity_with_UUID(UUID(), name); }

	entity map::create_entity_with_UUID(UUID uuid, const std::string& name) {

		entity loc_entity = { m_registry.create(), this };
		loc_entity.add_component<ID_component>(uuid);
		loc_entity.add_component<tag_component>(name.empty() ? "Entity" : name);
		loc_entity.add_component<transform_component>();

		m_entity_map[uuid] = loc_entity;
		return loc_entity;
	}

	void map::destroy_entity(entity entity) {

		m_entity_map.erase(entity.get_UUID());
		m_registry.destroy(entity);
	}

	entity map::duplicate_entity(entity other) {

		std::string name = other.get_name();
		entity new_entity = create_entity(name);
		copy_component_if_exists(all_components{}, new_entity, other);
		return new_entity;
	}

	entity map::find_entity_by_name(std::string_view name) {

		auto view = m_registry.view<tag_component>();
		for (auto loc_entity : view) {
			const tag_component& tc = view.get<tag_component>(loc_entity);
			if (tc.tag == name)
				return entity{ loc_entity, this };
		}
		return {};
	}

	entity map::get_entity_by_UUID(UUID uuid) {

		if (m_entity_map.find(uuid) != m_entity_map.end())
			return { m_entity_map.at(uuid), this };

		return {};
	}

	void map::pause(bool should_pause) {

		if (m_system_state != system_state::inactive)
			m_system_state = should_pause ? system_state::suspended : system_state::active;
	}

	// =============================================================== runtime/simulation ===============================================================

	void map::on_runtime_start() { }

	void map::on_runtime_stop() { }

	void map::on_simulation_start() { }

	void map::on_simulation_stop() { }

}
