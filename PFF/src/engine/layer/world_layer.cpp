
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

		for (auto& obj : m_current_map->get_all_game_objects())
			obj.transform_2D.rotation = obj.transform_2D.rot(delta_time);

	}

	void world_layer::on_event(event& event) {
	}

	void world_layer::on_imgui_render() {
	}

}
