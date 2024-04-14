
#include <util/pch_editor.h>
//#include <engine/Events/Event.h>

#include "application.h"
#include "editor_inputs.h"
#include "engine/layer/world_layer.h"

#include "editor_controller.h"


namespace PFF {

	editor_controller::editor_controller() {

		LOG(Trace, "init");
		m_input_mapping = register_mapping<editor_inputs>();
	}

	editor_controller::~editor_controller() {

		LOG(Trace, "shutdown");
	}

	void editor_controller::update(const f32 delta) {

		static f32 move_speed = 50.0f;

		//if (m_input_mapping->capture_mouse.get_data<bool>()) { }
		if (m_input_mapping->capture_mouse.data.boolean) {

			application::get().capture_cursor();
			m_camera_direction.y += m_input_mapping->look.data.vec_2D.x * 0.1f * delta;
			m_camera_direction.x += m_input_mapping->look.data.vec_2D.y * 0.1f * delta;

			if (m_input_mapping->change_move_speed.data.vec_1D != 0)
				move_speed = std::clamp(move_speed += (move_speed * 0.1f * m_input_mapping->change_move_speed.data.vec_1D), 10.0f, 1000.0f);

			float yaw = m_camera_direction.y;
			const glm::vec3 forwardDir{ sin(-yaw), m_camera_direction.x, cos(yaw) };
			const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };

			m_camera_pos += forwardDir * (m_input_mapping->move.data.vec_3D.x * move_speed * delta);					// Move forward	/backward
			m_camera_pos += rightDir * (m_input_mapping->move.data.vec_3D.y * move_speed * delta);						// Move right	/left
			m_camera_pos += glm::vec3{ 0.f, -1.f, 0.f } *(m_input_mapping->move.data.vec_3D.z * move_speed * delta);	// Move up		/down

		} else
			application::get().release_cursor();

		if (m_input_mapping->toggle_fps.data.boolean)
			application::get().limit_fps(!application::get().get_limit_fps(), application::get().get_target_fps());

		//LOG(Trace, vec_to_str(camera_direction, "dir") << vec_to_str(camera_pos, "pos") << vec_to_str(m_input_mapping->move.data._3D, "input"));
		//m_world_layer->get_editor_camera()->set_view_XYZ(m_camera_pos, m_camera_direction);
	}
}
