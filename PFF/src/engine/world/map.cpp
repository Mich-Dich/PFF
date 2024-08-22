
#include "util/pffpch.h"

#include "application.h"
#include "components.h"
#include "entity_script.h"
#include "entity.h"
#include "engine/resource_management/static_mesh_asset_manager.h"

// serislization
#include "engine/resource_management/headers.h"

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

		m_path = application::get().get_project_path() / CONTENT_DIR / "worlds" / "test_map.pffasset";
		CORE_LOG(Trace, "Loading map: " << m_path);

		serialize(serializer::option::load_from_file);
				
		//class test_script : public entity_script {
		//public:
		//	void on_create() override { CORE_LOG(Debug, "Creating test_script instance") }
		//	void on_destroy() override { }
		//	void on_update(f32 delta_time) override { CORE_LOG(Info, "Time: " << delta_time); }
		//};
		//loc_entitiy.add_script_component<test_script>();

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
		loc_random.get_f32(0.f, 2 * glm::pi<f32>());


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

	map::~map() { 
	
		serialize(serializer::option::save_to_file);
	}

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

		// remove refrence to entity from any relationship
		if (entity.has_component<relationship_component>()) {

			auto& relationship = entity.get_component<relationship_component>();

			//auto it = std::remove(relationship.children_ID.begin(), relationship.children_ID.end(), entity.get_UUID());
			//if (it != relationship.children_ID.end())
			//	relationship.children_ID.erase(it);

			auto& parent_relationship = get_entity_by_UUID(relationship.parent_ID).get_component<relationship_component>();
			auto it = std::remove(parent_relationship.children_ID.begin(), parent_relationship.children_ID.end(), entity.get_UUID());
			if (it != parent_relationship.children_ID.end())
				parent_relationship.children_ID.erase(it);

			// remove entity from children
			if (relationship.children_ID.size()) {
				for (auto child_ID : relationship.children_ID) {

					auto child_entity = get_entity_by_UUID(child_ID);
					auto& child_relationship = child_entity.get_component<relationship_component>();
					child_relationship.parent_ID = relationship.parent_ID;
					parent_relationship.children_ID.push_back(child_ID);
				}
			}

		}

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

//#define RUNTIME_IMPLEMENTED

	void map::on_runtime_start() {
	
#ifdef RUNTIME_IMPLEMENTED
		// inatnciate every script		TODO: optimize load time by only loading what is needed
		m_registry.view<script_component>().each([=](auto entity, auto& script_comp) {

			script_comp.instance = script_comp.create_script();
			script_comp.instance->m_entity = PFF::entity{ entity, this };
			script_comp.instance->on_create();
		});
#endif // RUNTIME_IMPLEMENTED
	}

	void map::on_runtime_stop() {

#ifdef RUNTIME_IMPLEMENTED
		// Destroy every script that is still running
		m_registry.view<script_component>().each([=](auto entity, auto& script_comp) {
			script_comp.destroy_script(script_comp);
		});
#endif // RUNTIME_IMPLEMENTED
	}

	void map::on_simulation_start() { }

	void map::on_simulation_stop() { }

	void map::on_update(const f32 delta_time) {

#ifdef RUNTIME_IMPLEMENTED
		m_registry.view<script_component>().each([=](auto entity, auto& script_comp) {
			script_comp.instance->on_update(delta_time);
		});
#else
		m_registry.view<script_component>().each([=](auto entity, auto& script_comp) {
			
			// ==================== DEV-ONLY (move to on_runtime_start()) ====================
			if (!script_comp.instance) {

				script_comp.instance = script_comp.create_script();
				script_comp.instance->m_entity = PFF::entity{ entity, this };
				script_comp.instance->on_create();
			}

			script_comp.instance->on_update(delta_time);

		});
#endif // RUNTIME_IMPLEMENTED
	}


#define SERIALIZE_SIMPLE_COMPONENT(name, function)																							\
	if (loc_entity.has_component<##name##_component>()) {																					\
		auto& ##name##_comp = loc_entity.get_component<##name##_component>();																\
		entity_section.sub_section(std::string(#name) + "_component", [&](serializer::yaml& component_section) {							\
																																			\
			component_section##function																										\
		});																																	\
	}

#define DESERIALIZE_SIMPLE_COMPONENT(name, function)																						\
	entity_section.sub_section(std::string(#name) + "_component", [&](serializer::yaml& component_section) {								\
																																			\
		name##_component name##_comp{};																										\
		component_section##function																											\
																																			\
		loc_entity.add_component<name##_component>(name##_comp);																			\
	})




	void map::serialize(serializer::option option) {

		asset_file_header file_header{};
		file_header.version = version(1, 0, 0);
		file_header.type = file_type::map;
		file_header.timestamp = util::get_system_time();

		auto serializer = serializer::yaml(m_path, "map_data", option);
		serializer.sub_section("file_header", [&](serializer::yaml& header_section) {

			header_section.entry(KEY_VALUE(file_header.version))
				.entry(KEY_VALUE(file_header.type))
				.entry(KEY_VALUE(file_header.timestamp));
		});
	
		if (option == serializer::option::save_to_file) {

			const auto view = m_registry.view<entt::entity>();
			std::vector<entt::entity> entities(view.begin(), view.end());			// convert to vector<> because of serializer API
			std::reverse(entities.begin(), entities.end());							// reverse because EnTT counts from last to first in view<>()

			serializer.vector("entities", entities, [&](serializer::yaml& entity_section, u64 x) {

				entity loc_entity = entity(entities[x], this);

				auto& tag_comp = loc_entity.get_component<tag_component>();
				entity_section.entry(KEY_VALUE(tag_comp.tag));

				auto& ID_comp = loc_entity.get_component<ID_component>();
				entity_section.entry(KEY_VALUE(ID_comp.ID));

				auto& transform_comp = loc_entity.get_component<transform_component>();
				entity_section.sub_section("transform_component", [&](serializer::yaml& component_section) {

					glm::vec3 translation, rotation, scale;
					math::decompose_transform((const glm::mat4&)transform_comp, translation, rotation, scale);

					component_section.entry(KEY_VALUE(translation))
					.entry(KEY_VALUE(rotation))
					.entry(KEY_VALUE(scale));
				});

				SERIALIZE_SIMPLE_COMPONENT(mesh,
					.entry(KEY_VALUE(mesh_comp.asset_path))
					.entry(KEY_VALUE(mesh_comp.mobility))
					.entry(KEY_VALUE(mesh_comp.shoudl_render));
				);


				SERIALIZE_SIMPLE_COMPONENT(relationship,
					.entry(KEY_VALUE(relationship_comp.parent_ID))
					.vector(KEY_VALUE(relationship_comp.children_ID), [&](serializer::yaml& children, u64 x) {

						children.entry("ID", relationship_comp.children_ID[x]);
					});
				);


			});


		} else {		// LOAD

			std::vector<entt::entity> entities{};

			serializer.vector("entities", entities, [&](serializer::yaml& entity_section, u64 x) {
				

				std::string tag{};
				entity_section.entry(KEY_VALUE(tag));

				UUID ID{};
				entity_section.entry(KEY_VALUE(ID));


				entity loc_entity = create_entity_with_UUID(ID, tag);

				auto& transform_comp = loc_entity.get_component<transform_component>();
				entity_section.sub_section("transform_component", [&](serializer::yaml& component_section) {
					
					glm::vec3 translation, rotation, scale;

					component_section.entry(KEY_VALUE(translation))
					.entry(KEY_VALUE(rotation))
					.entry(KEY_VALUE(scale));

					math::compose_transform((glm::mat4&)transform_comp, translation, rotation, scale);

				});

				entity_section.sub_section("mesh_component", [&](serializer::yaml& component_section) {
					
					mesh_component mesh_comp{};
					component_section.entry(KEY_VALUE(mesh_comp.asset_path))
					.entry(KEY_VALUE(mesh_comp.mobility))
					.entry(KEY_VALUE(mesh_comp.shoudl_render));

					loc_entity.add_mesh_component(mesh_comp);
				});

				DESERIALIZE_SIMPLE_COMPONENT(relationship,
					.entry(KEY_VALUE(relationship_comp.parent_ID))
					.vector(KEY_VALUE(relationship_comp.children_ID), [&](serializer::yaml& children, u64 x) {

						children.entry("ID", relationship_comp.children_ID[x]);
					});
				);


			});

		}

	}

}
