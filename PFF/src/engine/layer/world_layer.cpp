
#include "util/pffpch.h"

#include "engine/Events/Event.h"
#include "world_layer.h"

namespace PFF {

	world_layer::world_layer() {

		m_current_map = std::make_shared<game_map>();
	}

	world_layer::~world_layer() {

		m_current_map.reset();
	}

	void world_layer::on_attach() {
	}

	void world_layer::on_detach() {
	}

	void world_layer::on_update(f32 delta_time) {

		for (auto& obj : m_current_map->get_all_game_objects()) {

			obj.transform.rotation += glm::mod(obj.rotation_speed * delta_time, two_pi<float>());
		}
	}

	void world_layer::on_event(event& event) {

		if(m_player_controller)
			m_player_controller->handle_event(event);
	}

	void world_layer::on_imgui_render() {
	}

	void world_layer::register_player_controller(std::shared_ptr<player_controller> player_controller) {

		m_player_controller = player_controller;
	}

}
