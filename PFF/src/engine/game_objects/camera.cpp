
#include "util/pffpch.h"
#include <limits>

#include "camera.h"

namespace PFF {


	camera::camera() {
	}

	camera::~camera() {
	}

	void camera::set_orthographic_projection(f32 left, f32 right, f32 top, f32 bottom, f32 near, f32 far) {

		m_projection_matrix= glm::mat4{ 1.0f };
		m_projection_matrix[0][0] = 2.f / (right - left);
		m_projection_matrix[1][1] = 2.f / (bottom - top);
		m_projection_matrix[2][2] = 1.f / (far - near);
		m_projection_matrix[3][0] = -(right + left) / (right - left);
		m_projection_matrix[3][1] = -(bottom + top) / (bottom - top);
		m_projection_matrix[3][2] = -near / (far - near);
	}

	void camera::set_perspective_projection(f32 fov_y, f32 aspect_ratio, f32 near, f32 far) {

		CORE_ASSERT(glm::abs(aspect_ratio - std::numeric_limits<float>::epsilon()) > 0.0f, "", "Aspect ratio check failed: [aspect_ratio - epsilon] > 0.0f");
		const float tanHalfFovy = tan(fov_y / 2.f);
		m_projection_matrix= glm::mat4{ 0.0f };
		m_projection_matrix[0][0] = 1.f / (aspect_ratio * tanHalfFovy);
		m_projection_matrix[1][1] = 1.f / (tanHalfFovy);
		m_projection_matrix[2][2] = far / (far - near);
		m_projection_matrix[2][3] = 1.f;
		m_projection_matrix[3][2] = -(far * near) / (far - near);
	}

}
