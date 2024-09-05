
#include "util/pffpch.h"

#include "application.h"
#include "engine/world/components.h"

#include "procedural_mesh_script.h"

namespace PFF {

	procedural_mesh_script::procedural_mesh_script() { }

	procedural_mesh_script::~procedural_mesh_script() { GET_RENDERER.cleanup_procedural_mesh(m_mesh_asset); }

	void procedural_mesh_script::clear() {

		m_mesh_asset.surfaces.clear();
		m_mesh_asset.vertices.clear();
		m_mesh_asset.indices.clear();
		m_mesh_asset.bounds = PFF::geometry::bounds();
	}

	void procedural_mesh_script::add_vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv, const glm::vec4& color) {
	
		m_mesh_asset.vertices.emplace_back(position, normal, color, uv);
	}

	void procedural_mesh_script::add_vertex(const u32 index, const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv, const glm::vec4& color) {
	
		if (m_mesh_asset.vertices.size() - 1 > index)
			m_mesh_asset.vertices.resize(index + 10);

		m_mesh_asset.vertices[index] = geometry::vertex(position, normal, color, uv);
	}

	void procedural_mesh_script::add_triangle(const u32 index1, const u32 index2, const u32 index3) {
		
		m_mesh_asset.indices.push_back(index1);
		m_mesh_asset.indices.push_back(index2);
		m_mesh_asset.indices.push_back(index3);
	}

	void procedural_mesh_script::add_quad(const u32 top_right_index, u32 top_left_index, const u32 bottom_left_index, const u32 bottom_right_index) {
	
		add_triangle(top_right_index, top_left_index, bottom_left_index);
		add_triangle(top_right_index, bottom_left_index, bottom_right_index);
	}
	
	void procedural_mesh_script::add_surface(const u32 start_index, const u32 index_count) {

		geometry::Geo_surface new_surface;
		new_surface.startIndex = start_index;
		new_surface.count = index_count;
		m_mesh_asset.surfaces.push_back(new_surface);
		m_mesh_asset.calc_bounds();						// TODO: change so it can be called for a single surface
	}

	void procedural_mesh_script::set_vertex_color(const u32 index, const glm::vec4& color) { }

	void procedural_mesh_script::recalculate_normals() { }

	void procedural_mesh_script::recalculate_bounds() { m_mesh_asset.calc_bounds(); }

	void procedural_mesh_script::add_debug_line(geometry::vertex start, geometry::vertex end, bool vertex_in_global_space) {
	
		if (!vertex_in_global_space) {

			GET_RENDERER.add_debug_line(start, end);
			return;
		}

		glm::mat4& entity_transform = get_component<transform_component>();
		glm::vec4 start_transformed = entity_transform * glm::vec4(start.position, 1.0f);
		glm::vec4 end_transformed = entity_transform * glm::vec4(end.position, 1.0f);
		start.position = glm::vec3(start_transformed);
		end.position = glm::vec3(end_transformed);

		// transform the normals
		glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(entity_transform)));
		start.normal = normal_matrix * start.normal;
		end.normal = normal_matrix * end.normal;

		GET_RENDERER.add_debug_line(start, end);
	}

	void procedural_mesh_script::add_debug_line(glm::vec3 start, glm::vec3 end, bool vertex_in_global_space) {

		PFF::geometry::vertex start_vert = {};
		start_vert.position = start;

		PFF::geometry::vertex end_vert = {};
		end_vert.position = end;

		add_debug_line(start_vert, end_vert, vertex_in_global_space);
	}

	void procedural_mesh_script::add_debug_cross(glm::vec3 position, f32 size, bool vertex_in_global_space) {

		add_debug_line(glm::vec3(position.x -size, position.y + 0, position.z + 0), glm::vec3(position.x + size, position.y + 0, position.z + 0), vertex_in_global_space);
		add_debug_line(glm::vec3(position.x + 0, position.y -size, position.z + 0), glm::vec3(position.x + 0, position.y + size, position.z + 0), vertex_in_global_space);
		add_debug_line(glm::vec3(position.x + 0, position.y + 0, position.z -size), glm::vec3(position.x + 0, position.y + 0, position.z + size), vertex_in_global_space);
	}

	void procedural_mesh_script::clear_debug_line() {		GET_RENDERER.clear_debug_line(); }

	void procedural_mesh_script::apply_mesh(const bool recalc_bounds) {

		if (recalc_bounds)
			m_mesh_asset.calc_bounds();

		GET_RENDERER.update_mesh(m_mesh_asset, m_mesh_asset.indices, m_mesh_asset.vertices);
	}

}
