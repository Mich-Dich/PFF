#pragma once

#define GLM_FORCE_READIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace PFF {

	class PFF_API camera {
	public:
		camera();
		~camera();

		FORCEINLINE const glm::mat4& get_projection() const { return m_projection_matrix; }
		FORCEINLINE const glm::mat4& get_view() const { return view_matrix; }

		void set_view_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void set_view_YXZ(glm::vec3 position, glm::vec3 rotation);
		void set_view_XYZ(glm::vec3 position, glm::vec3 rotation);

		void set_orthographic_projection(f32 left, f32  right, f32 top, f32 bottom, f32 near, f32 far);
		void set_perspective_projection(f32 fov_y,f32 aspect_ratio, f32 near, f32 far);

	private:
		glm::mat4 m_projection_matrix{ 1.0f };
		glm::mat4 view_matrix{ 1.0f };
	};

}
