
#include "util/pffpch.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "math.h"

namespace PFF::math {

	bool decompose_transform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale) {
		// From glm::decompose in matrix_decompose.inl

		using T = float;

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