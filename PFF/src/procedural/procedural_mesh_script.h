#pragma once

#include "util/pffpch.h"

#include "engine/world/entity_script.h"
#include "engine/geometry/mesh.h"

namespace PFF {
	
	struct procedural_mesh_component;

	class procedural_mesh_script : public entity_script {
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
		void clear();
		void add_vertex(const glm::vec3& position, const glm::vec3& normal = {}, const glm::vec2& uv = {}, const glm::vec4& color = glm::vec4{ 1.f });
		void add_vertex(const u32 index, const glm::vec3& position, const glm::vec3& normal = {}, const glm::vec2& uv = {}, const glm::vec4& color = glm::vec4{ 1.f });
		void add_triangle(const u32 index_1, const u32 index_2, const u32 index_3);
		void add_quad(const u32 top_right_index, u32 top_left_index, const u32 bottom_left_index, const u32 bottom_right_index);

		u64 add_cube(const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation = glm::vec3(0), const glm::vec4& color = glm::vec4(1.0f));
		u64 add_sphere(const glm::vec3& position = glm::vec3(0), const glm::vec3& scale = glm::vec3(1.f), const glm::vec3& rotation = glm::vec3(0), u32 resolution = 32, const glm::vec4& color = glm::vec4(1.f));
		u64 add_surface(const u32 start_index, const u32 index_count = 0);

		void set_vertex_color(const u32 index, const glm::vec4& color);
		void recalculate_normals();
		void recalculate_bounds();
		void add_debug_line(geometry::vertex start, geometry::vertex end, bool vertex_in_global_space = true);
		void add_debug_line(glm::vec3 start, glm::vec3 end, glm::vec4 color = glm::vec4{ 1 }, bool vertex_in_global_space = true);
		void add_debug_cross(glm::vec3 position, f32 size, glm::vec4 color = glm::vec4{ 1 }, bool vertex_in_global_space = true);
		void show_debug_bounds(PFF::geometry::bounds bounds);
		void clear_debug_line();

		void sweep_profile_along_spline(/* PFF::spline */);

		void apply_mesh(const bool recalc_bounds = false);			// Apply the generated mesh data to the actual mesh component

		PFF::geometry::procedural_mesh_asset		m_mesh_asset = PFF::geometry::procedural_mesh_asset();

	private:

		//void find_mesh_intersections(const geometry::procedural_mesh_asset& mesh1, const geometry::procedural_mesh_asset& mesh2, std::vector<std::pair<u32, u32>>& intersections);
		//void cut_mesh_along_intersections(geometry::procedural_mesh_asset& mesh, const std::vector<std::pair<u32, u32>>& intersections);
		//void remove_interior_triangles(geometry::procedural_mesh_asset& mesh, const geometry::procedural_mesh_asset& subtractor);
		//void merge_meshes(geometry::procedural_mesh_asset& mesh1, const geometry::procedural_mesh_asset& mesh2);

		friend struct procedural_mesh_component;
		friend class map;

	};

}

//ref<PFF::geometry::mesh_asset> create_empty_mesh();
//void add_cube(const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation = glm::vec3(0), const glm::vec4& color = glm::vec4(1.0f));
//void add_sphere(const glm::vec3& position = glm::vec3(0), const glm::vec3& scale = glm::vec3(1.f), const glm::vec3& rotation = glm::vec3(0), u32 resolution = 32, const glm::vec4& color = glm::vec4(1.f));

//create_empty_mesh().add_cube()
