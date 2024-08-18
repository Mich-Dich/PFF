
#include "util/pffpch.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "math.h"

namespace PFF::math {
	
	bool is_valid_vec3(const glm::vec3& vec) { return !std::isnan(vec.x) && !std::isnan(vec.y) && !std::isnan(vec.z); }

	bool decompose_transform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale) {
		// From glm::decompose in matrix_decompose.inl

		using T = f32;

		glm::mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<T>(0), glm::epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), glm::epsilon<T>()) ||
			glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), glm::epsilon<T>()) ||
			glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), glm::epsilon<T>())) {
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = glm::vec3(LocalMatrix[3]);
		LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

		glm::vec3 Row[3];

		// Now get scale and shear.
		for (glm::length_t i = 0; i < 3; ++i)
			for (glm::length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		glm::vec3 Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0) {
			for (length_t i = 0; i < 3; i++) { 
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		} else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}


		return true;
	}
	/*
	glm::mat4 mat = matrix;

		// Extract translation
		translation = glm::vec3(mat[3]);

		// Normalize the right and up vectors
		glm::vec3 right = glm::normalize(glm::vec3(mat[0]));
		glm::vec3 up = glm::normalize(glm::vec3(mat[1]));
		glm::vec3 forward = glm::normalize(glm::vec3(mat[2]));

		// Extract scale
		scale.x = glm::length(glm::vec3(mat[0]));
		scale.y = glm::length(glm::vec3(mat[1]));
		scale.z = glm::length(glm::vec3(mat[2]));

		// Handle negative scaling
		if (glm::dot(glm::cross(right, up), forward) < 0.0f) {
			scale.x = -scale.x;
		}

		// Extract rotation
		glm::mat4 rotMat = glm::mat4(1.0f);
		rotMat[0] = glm::vec4(right, 0.0f);
		rotMat[1] = glm::vec4(up, 0.0f);
		rotMat[2] = glm::vec4(forward, 0.0f);

		// Decompose rotation from rotation matrix
		glm::quat rotationQuat = glm::quat_cast(rotMat);
		rotation = glm::eulerAngles(rotationQuat);
	*/


	bool compose_transform(glm::mat4& transform, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale) {

		if (!is_valid_vec3(translation) || !is_valid_vec3(rotation) || !is_valid_vec3(scale) ||
			scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) {
			return false;
		}

		glm::mat4 translation_matrix = glm::mat4(1.0f);
		glm::mat4 scale_matrix = glm::mat4(1.0f);
		glm::mat4 rotation_matrix = glm::mat4(1.0f);

		translation_matrix[3] = glm::vec4(translation.x, translation.y, translation.z, 1.0f);

		glm::quat quaternion(rotation);
		rotation_matrix = glm::toMat4(quaternion);

		scale_matrix[0][0] = scale.x;
		scale_matrix[1][1] = scale.y;
		scale_matrix[2][2] = scale.z;

		transform = translation_matrix * rotation_matrix * scale_matrix;
		return true;
	}


	f32 calc_array_average(const f32* array, u32 size) {

		if (size <= 0)
			return 0.f;

		__m128 sum = _mm_setzero_ps();
		u32 i = 0;

		for (; i <= size - 4; i += 4) {                     // Process 4 floats at a time
			__m128 values = _mm_loadu_ps(&array[i]);
			sum = _mm_add_ps(sum, values);
		}

		// Horizontal addition to get the total sum
		f32 temp[4];
		_mm_storeu_ps(temp, sum);
		f32 total_sum = temp[0] + temp[1] + temp[2] + temp[3];

		for (; i < size; ++i)                               // Handle remaining elements
			total_sum += array[i];

		return total_sum / size;
	}
	
	f32 calc_array_max(const f32* array, u32 size) {

		if (size <= 0)
			return 0.f;

		__m128 max = _mm_set1_ps(-FLT_MAX);

		u32 i = 0;
		for (; i <= size - 4; i += 4) {                     // Process 4 floats at a time
			__m128 values = _mm_loadu_ps(&array[i]);
			max = _mm_max_ps(max, values);
		}

		f32 temp[4];
		_mm_storeu_ps(temp, max);

		f32 max_value = temp[0];
		for (u32 x = 1; x < 4; ++x) {
			if (temp[x] > max_value) {
				max_value = temp[x];
			}
		}

		for (; i < size; ++i) {
			if (array[i] > max_value) {
				max_value = array[i];
			}
		}

		return max_value;
	}

}
