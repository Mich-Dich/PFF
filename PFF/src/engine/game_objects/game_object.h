#pragma once

#include "engine/geometry/basic_mesh.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace PFF {

	struct transform_comp {

		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation = {};

		glm::mat4 mat4();
		glm::mat4 mat4_XYZ();
		glm::mat3 normal_matrix();
	};

	class game_object {
	public:

		DELETE_COPY(game_object);

		game_object(game_object&&) = default;
		game_object& operator=(game_object&&) = default;

		FORCEINLINE u32 get_ID() const { return m_id; }
		FORCEINLINE void set_translation(const glm::vec3 translation) { transform.translation = translation; }

		// TODO: remove this 
		static game_object create_game_object() {

			static u32 current_id = 0;
			return game_object(current_id++);
		}

		ref<basic_mesh> mesh{};
		glm::vec3 color{};
		transform_comp transform{};
		glm::vec3 rotation_speed = {};

	private:

		game_object(u32 object_id) : m_id(object_id) {};

		u32 m_id;
	};

}
