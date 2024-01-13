
#include "util/pffpch.h"

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

			obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + delta_time, two_pi<float>());
			obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + (delta_time * .5f), two_pi<float>());
			obj.transform.rotation.z = glm::mod(obj.transform.rotation.z + (delta_time * .75f), two_pi<float>());
		}
	}

	void world_layer::on_event(event& event) {
	}

	void world_layer::on_imgui_render() {
	}

}
