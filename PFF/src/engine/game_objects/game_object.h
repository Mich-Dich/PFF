#pragma once

#include "engine/geometry/basic_mesh.h"

namespace PFF {

	struct transform_comp_2d {
		glm::vec2 translation;
		glm::vec2 scale{ 1.0f,1.0f };
		f32 rotation;

		glm::mat2 mat2() {
			const f32 sin = glm::sin(rotation);
			const f32 cos = glm::cos(rotation);

			glm::mat2 rotMat{ {cos,sin},{-sin,cos} };
			glm::mat2 scaleMat{ {scale.x, 0.0f},{0.0f, scale.y} };
			return rotMat * scaleMat;
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
