
#include "util/pffpch.h"

#include "application.h"
#include "engine/render/renderer.h"
#include "engine/Events/Event.h"
#include "world_layer.h"

namespace PFF {

	world_layer::world_layer() {

		m_current_map = std::make_shared<game_map>();
		m_editor_camera = std::make_shared<camera>();
		//m_editor_camera.set_orthographic_projection(-aspect, aspect, -1, 1, 0, 10);

		m_editor_camera->set_perspective_projection(glm::radians(50.f), application::get().get_renderer()->get_aspect_ratio(), 0.1f, 350.0f);
		m_editor_camera->set_view_direction(glm::vec3{ 0.0f }, glm::vec3{ 0.5f, 0.0f, 1.0f });

		//float aspect = m_swapchain->get_extentAspectRatio();
		//m_editor_camera.set_view_target(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(0.0f));
	}

	world_layer::~world_layer() {

		m_player_controller.reset();
		m_editor_camera.reset();
		m_current_map.reset();

		CORE_LOG(Info, "Shutdown");
	}

	void world_layer::on_attach() {
	}

	void world_layer::on_detach() {

		CORE_LOG(Info, "Detach");
	}

	void world_layer::on_update(f32 delta_time) {

		m_player_controller->update_internal(delta_time);

		// m_camera.set_orthographic_projection(-aspect, aspect, -1, 1, 0, 10);
		// m_camera.set_view_YXZ(glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(0.0f));
		// m_camera.set_view_direction(glm::vec3{ 0.0f }, glm::vec3{ 0.5f, 0.0f, 1.0f });
		m_editor_camera->set_perspective_projection(glm::radians(50.f), application::get().get_renderer()->get_aspect_ratio(), .1f, 350.0f);

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


}
