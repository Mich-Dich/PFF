
//#include <util/pffpch.h>
#include <engine/Events/Event.h>

#include "editor_inputs.h"
#include "editor_controller.h"
#include "engine/layer/world_layer.h"


namespace PFF {

	editor_controller::editor_controller() {

		LOG(Debug, "register Input mapping");
		m_input_mapping = register_mapping<editor_inputs>();
	}

	void editor_controller::update(f32 delta) {

		// camera_pos.z += camera_direction.x * m_input_mapping->move.data.axis_2d.x * 100 * delta;
		// camera_pos.x += camera_direction.y * m_input_mapping->move.data.axis_2d.y * 100 * delta;

		//glm::vec3 temp;
		//temp.x = camera_direction.z;
		//temp.y = camera_direction.x;
		//temp.z = camera_direction.y;

		//camera_pos += temp * (m_input_mapping->move.data.axis_2d.x * 50 * delta);
		//camera_pos += temp * (m_input_mapping->move.data.axis_2d.y * 50 * delta);

		 // Normalize the direction vector
		auto temp = camera_direction;
		glm::vec3 front = glm::dot(camera_direction, camera_direction) ? glm::normalize(camera_direction) : glm::vec3(0);

		// Compute the right and up vectors based on the direction
		glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::cross(right, front);

		// Update the position based on the movement direction
		//camera_pos += (m_input_mapping->move.data.axis_2d.y * 50 * delta) * right; // Move sideways
		//camera_pos += m_input_mapping->move.data.axis_2d.x * 50 * delta) * up;    // Move up/down

		float yaw = camera_direction.y;
		const glm::vec3 forwardDir{ sin(-yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		camera_pos += forwardDir * (m_input_mapping->move.data._3D.x * 0.5f); // Move forward	/backward
		camera_pos += forwardDir * (m_input_mapping->move.data._3D.y * 0.5f); // Move right		/left
		camera_pos += forwardDir * (m_input_mapping->move.data._3D.z * 0.5f); // Move up		/down

		f32 delts_x = (m_input_mapping->move.data._2D.x * 50 * delta);

		// camera_pos -= delts_x * glm::normalize(glm::vec3(m_world_layer->get_editor_camera()->get_view()[2]));

		if (m_input_mapping->capture_mouse.data.boolean) {

			camera_direction.y += m_input_mapping->look.data._2D.x * 0.1f * delta;
			camera_direction.x += m_input_mapping->look.data._2D.y * 0.1f * delta;
		}

		LOG(Trace, "dir: " << vec_to_str(camera_direction) << "pos: " << vec_to_str(camera_pos));
		m_world_layer->get_editor_camera()->set_view_XYZ(camera_pos, camera_direction);

		//std::shared_ptr<editor_controls> mapping = m_input_mapping;

	}
}
