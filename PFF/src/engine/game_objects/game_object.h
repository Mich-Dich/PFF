#pragma once

#include "engine/geometry/basic_mesh.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace PFF {

	struct transform_comp_2d {

		glm::vec2 translation;
		glm::vec2 scale{ 1.0f,1.0f };
		f32 rotation = 0.0f;
		f32 rotation_speed = 0.0f;

		glm::mat2 mat2() {
			const f32 sin = glm::sin(rotation);
			const f32 cos = glm::cos(rotation);

			glm::mat2 rotMat{ {cos,sin},{-sin,cos} };
			glm::mat2 scaleMat{ {scale.x, 0.0f},{0.0f, scale.y} };
			return rotMat * scaleMat;
		}

		f32 rot(f32 delta_time) {
			return glm::mod(rotation + (rotation_speed * delta_time), two_pi<float>());
		}
	};

	class game_object {

	public:

		DELETE_COPY(game_object);

		FORCEINLINE u32 get_ID() const { return m_id; }

		game_object(game_object&&) = default;
		game_object& operator=(game_object&&) = default;



		// TODO: remove this 
		static game_object create_game_object() {

			static u32 current_id = 0;
			return game_object(current_id++);
		}

		std::shared_ptr<basic_mesh> mesh{};
		glm::vec3 color{};
		transform_comp_2d transform_2D{};

	private:

		game_object(u32 object_id) : m_id(object_id) {};

		u32 m_id;
	};

}
