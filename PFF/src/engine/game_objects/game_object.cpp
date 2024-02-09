
#include "util/pffpch.h"

#include "game_object.h"

namespace PFF {



    glm::mat4 transform_comp::mat4() {

		PFF_PROFILE_FUNCTION();

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

	glm::mat4 transform_comp::mat4_XYZ() {

		PFF_PROFILE_FUNCTION();

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


	glm::mat3 transform_comp::normal_matrix() {

		PFF_PROFILE_FUNCTION();

		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 inverse_scale = 1.f / scale;

		return glm::mat3{
			{
				inverse_scale.x * (c1 * c3 + s1 * s2 * s3),
				inverse_scale.x * (c2 * s3),
				inverse_scale.x * (c1 * s2 * s3 - c3 * s1)
			},
			{
				inverse_scale.y* (c3 * s1 * s2 - c1 * s3),
				inverse_scale.y* (c2 * c3),
				inverse_scale.y * (c1 * c3 * s2 + s1 * s3),
			},
			{
				inverse_scale.z* (c2 * s1),
				inverse_scale.z* (-s2),
				inverse_scale.z * (c1 * c2),
			},
		};
	}

}