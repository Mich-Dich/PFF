
#include "util/pffpch.h"

#include "application.h"
#include "engine/render/renderer.h"
#include "engine/Events/Event.h"
#include "world_layer.h"

namespace PFF {
	
	world_layer::world_layer() { 

		CORE_LOG(Error, "CONSTRUCTING WORLD_LAYER"); 

		m_maps.emplace_back( create_ref<map>() );

		CORE_LOG_INIT();
	}

	world_layer::~world_layer() { CORE_LOG_SHUTDOWN(); }


	void world_layer::register_player_controller(ref<player_controller> player_controller) {

		m_player_controller = player_controller;
		m_player_controller->set_world_layer_ref(this);
	}

	void world_layer::on_attach() {
	
		m_editor_camera = std::make_shared<camera>();
		m_editor_camera->set_view_direction(glm::vec3{ 0.0f }, glm::vec3{ 0.5f, 0.0f, 1.0f });
		//m_editor_camera.set_orthographic_projection(-aspect, aspect, -1, 1, 0, 10);
		//m_editor_camera->set_perspective_projection(glm::radians(50.f), application::get().get_renderer()->get_aspect_ratio(), 0.1f, 350.0f);

		//float aspect = m_swapchain->get_extentAspectRatio();
		//m_editor_camera.set_view_target(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(0.0f));
		
		CORE_LOG(Trace, "attaching world_layer");
	}

	void world_layer::on_detach() { 

		m_player_controller.reset();
		m_editor_camera.reset();
		
		CORE_LOG(Trace, "detaching world_layer");
	}

	void world_layer::on_update(const f32 delta_time) {

		PFF_PROFILE_FUNCTION();

		m_player_controller->update_internal(delta_time);
		
		//for (auto& obj : m_current_map->get_all_game_objects()) {
		//
		//	obj.second.transform.rotation += glm::mod(obj.second.rotation_speed * delta_time, two_pi<f32>());
		//	obj.second.transform.translation += obj.second.movement_speed * delta_time;
		//}
		
	}

	void world_layer::on_event(event& event) {

		PFF_PROFILE_FUNCTION();

		if (m_player_controller)
			m_player_controller->handle_event(event);
	}

	void world_layer::on_imgui_render() {
	

	}


}
