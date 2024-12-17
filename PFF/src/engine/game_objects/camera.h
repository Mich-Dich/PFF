#pragma once

#define GLM_FORCE_READIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace PFF {

	class camera {
	public:

		camera();
		~camera();

		FORCEINLINE glm::mat4& get_projection_NC() { return m_projection_matrix; }
		FORCEINLINE glm::mat4& get_view_NC() { return view_matrix; }


		FORCEINLINE const glm::mat4& get_projection() const { return m_projection_matrix; }
		FORCEINLINE const glm::mat4& get_view() const { return view_matrix; }
		FORCEINLINE const f32& get_perspective_fov_y() const { return m_perspective_fov_y; }

		FORCEINLINE void force_set_projection_matrix(const glm::mat4& projection) { m_projection_matrix = projection; }
		FORCEINLINE void force_set_view_matrix(const glm::mat4& view) { view_matrix = view; }

		void set_clipping_dis(const f32 near_dis, const f32 far_dis);
		void set_view_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void set_view_YXZ(glm::vec3 position, glm::vec3 rotation);
		void set_view_XYZ(glm::vec3 position, glm::vec3 rotation);

		void set_orthographic_projection(const f32 left, const  f32 right, const f32 top, const f32 bottom, const f32 near, const f32 far);
		void set_perspective_projection(const f32 fov_y, const f32 aspect_ratio, const f32 near, const f32 far);
		void set_aspect_ratio(f32 aspect_ratio);
		void set_fov_y(f32 fov_y);

		glm::vec3 m_position{};
		glm::vec3 m_direction{};

	private:

		FORCEINLINE void update_orthographic_projection() { set_orthographic_projection(m_orthographic_left, m_orthographic_right, m_orthographic_up, m_orthographic_down, m_clipping_near, m_clipping_far); }
		FORCEINLINE void update_perspective_projection() { set_perspective_projection(m_perspective_fov_y, m_perspective_aspect_ratio, m_clipping_near, m_clipping_far); }
		
		f32 m_perspective_fov_y = 70.f;
		f32 m_perspective_aspect_ratio{};
		f32 m_orthographic_left{};
		f32 m_orthographic_right{};
		f32 m_orthographic_up{};
		f32 m_orthographic_down{};

		f32 m_clipping_near = 0.1f;
		f32 m_clipping_far = 1000.0f;


		glm::mat4 m_projection_matrix{ 1.0f };
		glm::mat4 view_matrix{ 1.0f };
	};

}
