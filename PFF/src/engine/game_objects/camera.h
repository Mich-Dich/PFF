#pragma once

#define GLM_FORCE_READIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace PFF {

	class camera {
	public:
		camera();
		~camera();

		FORCEINLINE const glm::mat4& get_projection() const { return m_projection_matrix; }

		void set_orthographic_projection(f32 left, f32  right, f32 top, f32 bottom, f32 near, f32 far);
		void set_perspective_projection(f32 fov_y,f32 aspect_ratio, f32 near, f32 far);

	private:
		glm::mat4 m_projection_matrix{ 1.0f };
	};

}
