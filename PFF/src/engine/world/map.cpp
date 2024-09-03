
#include "util/pffpch.h"

#include "application.h"
#include "components.h"
#include "entity_script.h"
#include "entity.h"
#include "procedural/procedural_mesh_script.h"
#include "engine/resource_management/static_mesh_asset_manager.h"

// serislization
#include "engine/resource_management/headers.h"

#include "map.h"

#define DEV_ONLY
#ifdef DEV_ONLY
	#include "project/script_system.h"
#endif
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
	static void copy_component_if_exists(entity dst, entity src) {
		([&]() {
			if (src.has_component<component>())
				dst.add_or_replace_component<component>(src.get_component<component>());
			}(), ...);
	}

	template<typename... component>
	static void copy_component(component_group<component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& entt_map) { copy_component<component...>(dst, src, entt_map); }

	template<typename... component>
	static void copy_component_if_exists(component_group<component...>, entity dst, entity src) { copy_component_if_exists<component...>(dst, src); }

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

#ifdef DEV_ONLY

		//CORE_LOG(Debug, "create entity and add [simple_terrain_script]");
		//entity loc_entitiy = create_entity("procedural_script_test");
		//script_system::add_component_from_string("simple_terrain_script", loc_entitiy);

#endif












		//// ================================================ SIMPLE TERRAIN GENERATOR ===============================================
		//// procedural terrain test

		//geometry::mesh_asset loc_mesh{};
		//loc_mesh.name = "procedural_test";

		//PFF::util::noise noise(PFF::util::noise_type::perlin);
		//noise.Set_fractal_type(PFF::util::fractal_type::FBm);
		//noise.set_frequency(0.005f);
		//noise.set_fractal_octaves(4);
		//noise.set_fractal_lacunarity(2.3f);

		//const glm::ivec2 grid_size = glm::ivec2(5);			// number of grid tiles
		//const glm::vec2 grid_tile_size = glm::ivec2(100);	// size of a grid tile
		//const glm::vec2 grid_resolution = glm::ivec2(100);
		//const int iterations_x = static_cast<u32>(grid_size.x * grid_resolution.x);
		//const int iterations_y = static_cast<u32>(grid_size.y * grid_resolution.y);

		//const glm::vec2 offset = glm::vec2(
		//	(static_cast<f32>(grid_size.x) / 2) * grid_tile_size.x,
		//	(static_cast<f32>(grid_size.y) / 2) * grid_tile_size.y
		//);
		//const glm::vec2 pos_multiplier = glm::vec2(
		//	(grid_tile_size.x / grid_resolution.x),
		//	(grid_tile_size.y / grid_resolution.y)
		//);

		//loc_mesh.vertices.resize((iterations_x + 1) * (iterations_y + 1));
		//loc_mesh.indices.resize((iterations_x * iterations_y) * 6);

		//int counter = 0;
		//for (int y = 0; y <= iterations_y; ++y) {
		//	for (int x = 0; x <= iterations_x; ++x) {

		//		loc_mesh.vertices[counter] = geometry::vertex(
		//			glm::vec3(
		//			(x * pos_multiplier.x) - offset.x,
		//			noise.get_noise((f32)x, (f32)y) * 50.f,
		//			(y * pos_multiplier.y) - offset.y
		//		),
		//			{ 0, 0, 1 },
		//			glm::vec4{ 1.f },
		//			static_cast<f32>(x) / grid_resolution.x,
		//			static_cast<f32>(y) / grid_resolution.y
		//		);
		//		counter++;
		//	}
		//}

		//counter = 0;
		//for (int y = 0; y < iterations_y; y++) {
		//	for (int x = 0; x < iterations_x; x++) {

		//		int top_left = y * ((iterations_x)+1) + x;
		//		int top_right = top_left + 1;
		//		int bottom_left = (y + 1) * ((iterations_x)+1) + x;
		//		int bottom_right = bottom_left + 1;

		//		loc_mesh.indices[counter + 0] = top_left;
		//		loc_mesh.indices[counter + 1] = bottom_left;
		//		loc_mesh.indices[counter + 2] = top_right;

		//		loc_mesh.indices[counter + 3] = top_right;
		//		loc_mesh.indices[counter + 4] = bottom_left;
		//		loc_mesh.indices[counter + 5] = bottom_right;
		//		counter += 6;
		//	}
		//}

		//geometry::Geo_surface new_surface{};
		//new_surface.count = static_cast<int>(loc_mesh.indices.size());
		//loc_mesh.surfaces.push_back(new_surface);

		//loc_mesh.calc_bounds();


		//auto T_test_mesh = create_ref<geometry::mesh_asset>(std::move(loc_mesh));
		//T_test_mesh->mesh_buffers = GET_RENDERER.upload_mesh(T_test_mesh->indices, T_test_mesh->vertices);






		//entity terrain_entitiy = create_entity("Test entity for PG");
		//auto& transform_comp = terrain_entitiy.get_component<transform_component>();
		//auto& mesh_comp = terrain_entitiy.add_component<mesh_component>();
		//mesh_comp.mesh_asset = T_test_mesh;
		//mesh_comp.asset_path = std::filesystem::path("suff");

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

			auto& parent_relationship = get_entity_by_UUID(relationship.parent_ID).get_component<relationship_component>();
			auto it = std::remove(parent_relationship.children_ID.begin(), parent_relationship.children_ID.end(), entity.get_UUID());
			if (it != parent_relationship.children_ID.end())
				parent_relationship.children_ID.erase(it);

			// remove entity from children
			for (auto child_ID : relationship.children_ID) {

				auto child_entity = get_entity_by_UUID(child_ID);
				auto& child_relationship = child_entity.get_component<relationship_component>();
				child_relationship.parent_ID = relationship.parent_ID;
				parent_relationship.children_ID.push_back(child_ID);
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

		m_registry.view<procedural_mesh_component>().each([=](auto entity, auto& script_comp) {

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

				SERIALIZE_SIMPLE_COMPONENT(procedural_mesh,
					.entry(KEY_VALUE(procedural_mesh_comp.script_name))
					.entry(KEY_VALUE(procedural_mesh_comp.mobility))
					.entry(KEY_VALUE(procedural_mesh_comp.shoudl_render));
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

				entity_section.sub_section("procedural_mesh_component", [&](serializer::yaml& component_section) {

					procedural_mesh_component proc_mesh_comp{};
					component_section.entry(KEY_VALUE(proc_mesh_comp.script_name))
						.entry(KEY_VALUE(proc_mesh_comp.mobility))
						.entry(KEY_VALUE(proc_mesh_comp.shoudl_render));

					script_system::add_component_from_string(proc_mesh_comp.script_name, loc_entity);
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
