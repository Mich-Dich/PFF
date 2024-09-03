#pragma once

#include "util/pffpch.h"

#include "engine/world/entity_script.h"

#include "engine/geometry/mesh.h"

namespace PFF {

	struct procedural_mesh_component;
	
	class PFF_API procedural_mesh_script : public entity_script {
	public:

		procedural_mesh_script();
		virtual ~procedural_mesh_script();
		PFF_DEFAULT_GETTER(PFF::geometry::procedural_mesh_asset&,		mesh_asset);

		virtual void on_rebuild() {}
		virtual void on_collision_overlap() {}

	protected:   
		
		//virtual void on_create() {}
		//virtual void on_destroy() {}
		//virtual void on_update(f32 delta_time) {}

		// Helper functions for mesh generation
		FORCEINLINE void clear();
		FORCEINLINE void add_vertex(const glm::vec3& position, const glm::vec3& normal = {}, const glm::vec2& uv = {}, const glm::vec4& color = glm::vec4{ 1.f });
		FORCEINLINE void add_triangle(const u32 index_1, const u32 index_2, const u32 index_3);
		FORCEINLINE void add_quad(const u32 top_right_index, u32 top_left_index, const u32 bottom_left_index, const u32 bottom_right_index);
		FORCEINLINE void add_surface(const u32 start_index, const u32 index_count = 0);
		FORCEINLINE void set_vertex_color(const u32 index, const glm::vec4& color);
		FORCEINLINE void recalculate_normals();
		FORCEINLINE void recalculate_bounds();
		void apply_mesh(const bool recalc_bounds = false);			// Apply the generated mesh data to the actual mesh component

		PFF::geometry::procedural_mesh_asset		m_mesh_asset = PFF::geometry::procedural_mesh_asset();

	private:

		friend struct procedural_mesh_component;
		friend class map;

	};

}
