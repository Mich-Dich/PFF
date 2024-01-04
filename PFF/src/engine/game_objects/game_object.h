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

		using id_t = u32;

		FORCEINLINE id_t get_ID() const { return m_id; }

		static game_object create_game_object() {

			static id_t current_id = 0;
			return game_object(current_id++);
		}

		DELETE_COPY(game_object);
		game_object(game_object&&) = default;
		game_object& operator=(game_object&&) = default;

		std::shared_ptr<basic_mesh> mesh{};
		glm::vec3 color{};
		transform_comp_2d transform_2D{};

	private:

		game_object(id_t object_id) : m_id(object_id) {};

		id_t m_id;
	};

}
