
#include "util/pch_editor.h"

#include "PFF_editor.h"


namespace PFF {

	void PFF_editor::serialize(serializer::option option) {

		// list all serialize functions
		m_editor_controller->serialize(option);
	}

	PFF_editor::PFF_editor() {

		m_editor_layer = new editor_layer(application::get().get_imgui_layer()->get_context());
		push_overlay(m_editor_layer);

		LOG(Trace, "register editor controller");
		m_editor_controller = std::make_shared<editor_controller>();
		register_player_controller(m_editor_controller);
				
		serialize(serializer::option::load_from_file);	
	}

	PFF_editor::~PFF_editor() {
	
		serialize(serializer::option::save_to_file);

		pop_overlay(m_editor_layer);
		delete m_editor_layer;

		CORE_LOG_SHUTDOWN();
	}

	bool PFF_editor::init() { 
	
		LOG(Info, "initalizing the editor");






		// TODO: load the map specefied in the project settings as editor_start_world
		ref<map> loc_map = create_ref<map>();

		//entity loc_entitiy = loc_map->create_entity("editor_origin_grid");
		//auto& transform_comp = loc_entitiy.get_component<transform_component>();
		//transform_comp.translation = glm::vec3(0);
		//transform_comp.rotation = glm::vec3(0);

		//auto& mesh_comp = loc_entitiy.add_component<mesh_component>();

		//mesh_comp.mesh_asset = static_mesh_asset_manager::get().get_test_mesh();
		//mesh_comp.material = material_instance_asset_manager::get().get_default_material_pointer();

		get_world_layer()->add_map( loc_map );





		
		return true;
	}

	bool PFF_editor::shutdown() { return true; }

	bool PFF_editor::render(const f32 delta_time) { return true; }

	bool PFF_editor::update(const f32 delta_time) { return true; }
}
