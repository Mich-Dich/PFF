#pragma once

#include "engine/game_objects/player_controller.h"

namespace PFF {

	class editor_inputs;

	class editor_controller : public player_controller {
	public:

		editor_controller();
		~editor_controller();

		void update(f32 delta) override;

	private:
		
		std::shared_ptr<editor_inputs> m_input_mapping{};

		glm::vec3 m_camera_pos{ 0.0f };
		glm::vec3 m_camera_direction{ 0.0f, 0.0f, 0.0f };
	};

}
