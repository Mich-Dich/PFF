#pragma once

//#include "engine/geometry/basic_mesh.h"

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
	/*
	class game_object {
	public:

		using id_t = u64;
		using map = std::unordered_map<id_t, game_object>;

		// TODO: remove this 
		static game_object create_game_object() {

			static id_t current_id = 0;
			return game_object(current_id++);
		}

		//DELETE_COPY(game_object);
		game_object() = default;
		game_object(game_object&&) = default;
		game_object& operator=(game_object&&) = default;

		FORCEINLINE id_t get_ID() const { return m_id; }
		FORCEINLINE void set_translation(const glm::vec3 translation) { transform.translation = translation; }

		ref<basic_mesh> mesh{};
		glm::vec3 color{};
		transform_comp transform{};
		glm::vec3 rotation_speed = {};
		glm::vec3 movement_speed = {};

	private:

		game_object(id_t object_id) : m_id(object_id) {};

		id_t m_id;
	};
	*/
}
