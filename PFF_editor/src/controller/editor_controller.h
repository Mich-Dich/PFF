#pragma once

#include "engine/game_objects/player_controller.h"

namespace PFF {

	class editor_inputs;

	class editor_controller : public player_controller {
	public:

		editor_controller();
		~editor_controller();

		FORCEINLINE glm::vec3 get_editor_camera_pos()							const { return m_camera_pos; }
		FORCEINLINE glm::vec3 get_editor_camera_direction()						const { return m_camera_direction; }
		FORCEINLINE f32 get_move_speed()										const { return m_move_speed; }
		FORCEINLINE void set_move_speed(f32 new_move_speed)						{ m_move_speed = new_move_speed; }
		FORCEINLINE void set_editor_camera_pos(const glm::vec3 pos)				{ m_camera_pos = pos; }
		FORCEINLINE void set_editor_camera_direction(const glm::vec3 direction) { m_camera_direction = direction; }

		void update(const f32 delta) override;
		void serialize(serializer::option option);

	private:
		
		ref<editor_inputs> m_input_mapping{};

		glm::vec3 m_camera_pos{};
		glm::vec3 m_camera_direction{};
		f32 m_move_speed = 50.0f;
	};

}
