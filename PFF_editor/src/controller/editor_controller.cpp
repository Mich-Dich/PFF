
//#include <util/pffpch.h>
#include <engine/Events/Event.h>

#include "application.h"

#include "editor_inputs.h"
#include "editor_controller.h"
#include "engine/layer/world_layer.h"


namespace PFF {

	editor_controller::editor_controller() {

		LOG(Debug, "register Input mapping");
		m_input_mapping = register_mapping<editor_inputs>();
	}

	editor_controller::~editor_controller() {

		LOG(Info, "Shutdown");
	}

	void editor_controller::update(f32 delta) {

		static f32 move_speed = 50.0f;
		if (m_input_mapping->change_move_speed.data._1D != 0) 
			move_speed = std::clamp(move_speed += (move_speed * 0.1f * m_input_mapping->change_move_speed.data._1D), 10.0f, 1000.0f);

		float yaw = camera_direction.y;
		const glm::vec3 forwardDir{ sin(-yaw), camera_direction.x, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };

		camera_pos += forwardDir * (m_input_mapping->move.data._3D.x * move_speed * delta);						// Move forward	/backward
		camera_pos += rightDir * (m_input_mapping->move.data._3D.y * move_speed * delta);						// Move right		/left
		camera_pos += glm::vec3{ 0.f, -1.f, 0.f } * (m_input_mapping->move.data._3D.z * move_speed * delta);	// Move up		/down

		if (m_input_mapping->capture_mouse.data.boolean) {

			application::get().capture_cursor();
			camera_direction.y += m_input_mapping->look.data._2D.x * 0.1f * delta;
			camera_direction.x += m_input_mapping->look.data._2D.y * 0.1f * delta;
		} else
			application::get().release_cursor();

		if (m_input_mapping->toggle_fps.data.boolean)
			application::get().limit_fps(!application::get().get_limit_fps(), application::get().get_target_fps());
	
		//LOG(Trace, vec_to_str(camera_direction, "dir") << vec_to_str(camera_pos, "pos") << vec_to_str(m_input_mapping->move.data._3D, "input"));
		m_world_layer->get_editor_camera()->set_view_XYZ(camera_pos, camera_direction);

	}
}
