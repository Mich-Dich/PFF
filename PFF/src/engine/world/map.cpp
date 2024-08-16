
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
		CORE_LOG(Debug, "m_path: " << m_path);

		//{
		//	entity loc_entitiy = create_entity("editor_origin_grid");

		//	auto& transform_comp = loc_entitiy.get_component<transform_component>();
		//	transform_comp.translation = glm::vec3(0);
		//	transform_comp.rotation = glm::vec3(0);

		//	auto& mesh_comp = loc_entitiy.add_component<mesh_component>();			
		//	mesh_comp.mesh_asset = static_mesh_asset_manager::get_from_path("meshes/grass_002_Plane.pffasset");
		//	//mesh_comp.material = GET_RENDERER.get_default_material_pointer();		// get correct shader
		//}


		
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

	void map::serialize(serializer::option option) {

		asset_file_header file_header{};
		file_header.version = version(1, 0, 0);
		file_header.type = file_type::map;
		file_header.timestamp = util::get_system_time();

		serializer::yaml(m_path, "map_data", option)
			.sub_section("file_header", [&](serializer::yaml& header_section) {

				header_section.entry(KEY_VALUE(file_header.version))
				.entry(KEY_VALUE(file_header.type))
				.entry(KEY_VALUE(file_header.timestamp));
			})
			.sub_section("data", [&](serializer::yaml& data_section) {

				//const auto view = ;
				for (const auto entity_ID : m_registry.view<entt::entity>(/*entt::exclude<T>*/)) {

					data_section.sub_section(("entity_" + util::num_to_str((u32)entity_ID)), [&](serializer::yaml& entity_section) {

						entity loc_entity = entity(entity_ID, this);

						auto& tag_comp = loc_entity.get_component<tag_component>();
						entity_section.entry(KEY_VALUE(tag_comp.tag));

						auto& ID_comp = loc_entity.get_component<ID_component>();
						entity_section.entry(KEY_VALUE(ID_comp.ID));

						if (loc_entity.has_component<transform_component>()) {
							auto& transform_comp = loc_entity.get_component<transform_component>();
							entity_section.sub_section("transform_component", [&](serializer::yaml& component_section) {

								component_section.entry(KEY_VALUE(transform_comp.translation))
								.entry(KEY_VALUE(transform_comp.rotation))
								.entry(KEY_VALUE(transform_comp.scale));
							});
						}

						if (loc_entity.has_component<mesh_component>()) {
							auto& mesh_comp = loc_entity.get_component<mesh_component>();
							entity_section.sub_section("mesh_component", [&](serializer::yaml& component_section) {

								component_section.entry(KEY_VALUE(mesh_comp.asset_path))
								.entry(KEY_VALUE(mesh_comp.mobility))
								.entry(KEY_VALUE(mesh_comp.shoudl_render))
								.entry(KEY_VALUE(mesh_comp.transform));

							});
						}

					});

				}
			})
			;

	}

}
