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

		glm::mat4 mat4() {
			const float c1 = glm::cos(rotation.x);
			const float s1 = glm::sin(rotation.x);
			const float c2 = glm::cos(rotation.y);
			const float s2 = glm::sin(rotation.y);
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);

			return glm::mat4{
				{
					scale.x * (c2 * c3),
					scale.x * (c2 * s3),
					scale.x * (-s2),
					0.0f,
				},
				{
					scale.y * (c1 * s3 + c3 * s1 * s2),
					scale.y * (c1 * c3 - s1 * s2 * s3),
					scale.y * (c2 * s1),
					0.0f,
				},
				{
					scale.z * (s1 * s3 - c1 * c3 * s2),
					scale.z * (c3 * s1 + c1 * s2 * s3),
					scale.z * (c1 * c2),
					0.0f,
				},
				{translation.x, translation.y, translation.z, 1.0f}
			};
		}

		glm::mat4 mat4_XYZ() {

			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{translation.x, translation.y, translation.z, 1.0f}
			};
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
		transform_comp transform{};
		glm::vec3 rotation_speed = {};

	private:

		game_object(u32 object_id) : m_id(object_id) {};

		u32 m_id;
	};

}
