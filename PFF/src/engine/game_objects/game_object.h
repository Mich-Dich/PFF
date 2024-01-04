#pragma once

#include "engine/geometry/basic_mesh.h"

namespace PFF {

	struct transform_comp_2d {
		glm::vec2 translation;							// position offset
		glm::mat2 mat2() { return glm::mat2{ 1.0f }; }
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
		transform_comp_2d transform_3D{};

	private:

		game_object(id_t object_id) : m_id(object_id) {};

		id_t m_id;
	};

}