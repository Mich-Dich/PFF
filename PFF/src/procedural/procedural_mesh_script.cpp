
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

	void procedural_mesh_script::apply_mesh(const bool recalc_bounds) {

		if (recalc_bounds)
			m_mesh_asset.calc_bounds();

		GET_RENDERER.update_mesh(m_mesh_asset, m_mesh_asset.indices, m_mesh_asset.vertices);
	}

}
