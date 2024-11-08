
#include "util/pffpch.h"

#include "application.h"
#include "engine/world/components.h"
//#include "mesh_ops/voxel_grid.h"

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
	
	u64 procedural_mesh_script::add_surface(const u32 start_index, const u32 index_count) {

		geometry::Geo_surface new_surface;
		new_surface.startIndex = start_index;
		new_surface.count = index_count;
		m_mesh_asset.surfaces.push_back(new_surface);
		m_mesh_asset.calc_bounds();						// TODO: change so it can be called for a single surface
		return m_mesh_asset.surfaces.size() - 1;
	}
	//
	//u64 procedural_mesh_script::boolean_subtract(u64 main_mesh_index, u64 subtractor_mesh_index) {

	//	const auto& main_mesh = m_mesh_asset.surfaces[main_mesh_index];
	//	const auto& subtractor_mesh = m_mesh_asset.surfaces[subtractor_mesh_index];
	//	
	//	VoxelGrid main_grid(-main_mesh.bounds.extents, main_mesh.bounds.extents, 0.05f); // Adjust voxel size as needed
	//	VoxelGrid subtractor_grid(-subtractor_mesh.bounds.extents, subtractor_mesh.bounds.extents, 0.1f);

	//	main_grid.voxelize_mesh(m_mesh_asset, main_mesh_index);
	//	subtractor_grid.voxelize_mesh(m_mesh_asset, subtractor_mesh_index);

	//	main_grid.subtract(subtractor_grid);

	//	return main_grid.reconstruct_mesh(m_mesh_asset, -1);
	//}



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

	void procedural_mesh_script::add_debug_line(glm::vec3 start, glm::vec3 end, glm::vec4 color, bool vertex_in_global_space) {

		PFF::geometry::vertex start_vert = {};
		start_vert.position = start;
		start_vert.color = color;

		PFF::geometry::vertex end_vert = {};
		end_vert.position = end;
		end_vert.color = color;

		add_debug_line(start_vert, end_vert, vertex_in_global_space);
	}

	void procedural_mesh_script::add_debug_cross(glm::vec3 position, f32 size, glm::vec4 color, bool vertex_in_global_space) {

		add_debug_line(glm::vec3(position.x -size, position.y + 0, position.z + 0), glm::vec3(position.x + size, position.y + 0, position.z + 0), color, vertex_in_global_space);
		add_debug_line(glm::vec3(position.x + 0, position.y -size, position.z + 0), glm::vec3(position.x + 0, position.y + size, position.z + 0), color, vertex_in_global_space);
		add_debug_line(glm::vec3(position.x + 0, position.y + 0, position.z -size), glm::vec3(position.x + 0, position.y + 0, position.z + size), color, vertex_in_global_space);
	}

	u64 procedural_mesh_script::add_cube(const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation, const glm::vec4& color) {

		// Calculate the rotation matrix
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
		rotationMatrix = glm::rotate(rotationMatrix, rotation.y, glm::vec3(0, 1, 0));
		rotationMatrix = glm::rotate(rotationMatrix, rotation.z, glm::vec3(0, 0, 1));

		// Define the 8 vertices of the cube
		std::array<glm::vec3, 8> vertices = {
			glm::vec3(-0.5f, -0.5f, -0.5f),
			glm::vec3( 0.5f, -0.5f, -0.5f),
			glm::vec3( 0.5f,  0.5f, -0.5f),
			glm::vec3(-0.5f,  0.5f, -0.5f),
			glm::vec3(-0.5f, -0.5f,  0.5f),
			glm::vec3( 0.5f, -0.5f,  0.5f),
			glm::vec3( 0.5f,  0.5f,  0.5f),
			glm::vec3(-0.5f,  0.5f,  0.5f)
		};

		// Transform vertices
		for (auto& vertex : vertices)
			vertex = glm::vec3(rotationMatrix * glm::vec4(vertex * size, 1.0f)) + position;

		// Calculate UVs based on size
		std::array<glm::vec2, 4> uvs = {
			glm::vec2(0, 0),
			glm::vec2(size.x, 0),
			glm::vec2(size.x, size.y),
			glm::vec2(0, size.y)
		};

		// Add vertices to the mesh
		u32 baseIndex = (u32)m_mesh_asset.vertices.size();

		// Define the 6 faces of the cube with their corresponding normals and UV indices
		std::array<std::array<std::tuple<u32, glm::vec3, u32>, 4>, 6> faces = { {
			{{std::make_tuple(0, glm::vec3(0, 0, -1), 0), std::make_tuple(1, glm::vec3(0, 0, -1), 1), std::make_tuple(2, glm::vec3(0, 0, -1), 2), std::make_tuple(3, glm::vec3(0, 0, -1), 3)}}, // Front
			{{std::make_tuple(1, glm::vec3(1, 0, 0), 0), std::make_tuple(5, glm::vec3(1, 0, 0), 1), std::make_tuple(6, glm::vec3(1, 0, 0), 2), std::make_tuple(2, glm::vec3(1, 0, 0), 3)}}, // Right
			{{std::make_tuple(5, glm::vec3(0, 0, 1), 0), std::make_tuple(4, glm::vec3(0, 0, 1), 1), std::make_tuple(7, glm::vec3(0, 0, 1), 2), std::make_tuple(6, glm::vec3(0, 0, 1), 3)}}, // Back
			{{std::make_tuple(4, glm::vec3(-1, 0, 0), 0), std::make_tuple(0, glm::vec3(-1, 0, 0), 1), std::make_tuple(3, glm::vec3(-1, 0, 0), 2), std::make_tuple(7, glm::vec3(-1, 0, 0), 3)}}, // Left
			{{std::make_tuple(3, glm::vec3(0, 1, 0), 0), std::make_tuple(2, glm::vec3(0, 1, 0), 1), std::make_tuple(6, glm::vec3(0, 1, 0), 2), std::make_tuple(7, glm::vec3(0, 1, 0), 3)}}, // Top
			{{std::make_tuple(4, glm::vec3(0, -1, 0), 0), std::make_tuple(5, glm::vec3(0, -1, 0), 1), std::make_tuple(1, glm::vec3(0, -1, 0), 2), std::make_tuple(0, glm::vec3(0, -1, 0), 3)}}  // Bottom
		} };

		// Add vertices and triangles
		for (const auto& face : faces) {
			for (const auto& [vertexIndex, normal, uvIndex] : face) {
				glm::vec3 rotatedNormal = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(normal, 0.0f)));
				add_vertex(vertices[vertexIndex], rotatedNormal, uvs[uvIndex], color);
			}

			u32 faceBaseIndex = baseIndex + (u32)(&face - &faces[0]) * 4;
			add_quad(faceBaseIndex + 3, faceBaseIndex + 2, faceBaseIndex + 1, faceBaseIndex);
		}

		return add_surface((u32)m_mesh_asset.indices.size() - 36, 36);
	}
	
	u64 procedural_mesh_script::add_sphere(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation, u32 resolution, const glm::vec4& color) {

		// Calculate the rotation matrix
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
		rotationMatrix = glm::rotate(rotationMatrix, rotation.y, glm::vec3(0, 1, 0));
		rotationMatrix = glm::rotate(rotationMatrix, rotation.z, glm::vec3(0, 0, 1));

		// Calculate the number of vertices and indices
		const u32 vertexCount = (resolution + 1) * (resolution + 1);
		const u32 indexCount = resolution * resolution * 6;

		// Reserve space for new vertices and indices
		const u32 baseIndex = (u32)m_mesh_asset.vertices.size();
		m_mesh_asset.vertices.reserve(m_mesh_asset.vertices.size() + vertexCount);
		m_mesh_asset.indices.reserve(m_mesh_asset.indices.size() + indexCount);

		// generate vertices
		for (u32 y = 0; y <= resolution; ++y) {
			for (u32 x = 0; x <= resolution; ++x) {
				
				const f32 xSegment = (float)x / (float)resolution;
				const f32 ySegment = (float)y / (float)resolution;
				const f32 xPos = std::cos(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());
				const f32 yPos = std::cos(ySegment * glm::pi<float>());
				const f32 zPos = std::sin(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());

				glm::vec3 vertex = glm::vec3(xPos, yPos, zPos);
				vertex *= scale;
				vertex = glm::vec3(rotationMatrix * glm::vec4(vertex, 1.0f));
				vertex += position;

				glm::vec3 normal = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(glm::vec3(xPos, yPos, zPos), 0.0f)));
				glm::vec2 uv(xSegment, ySegment);

				add_vertex(vertex, normal, uv, color);
			}
		}

		// generate indices
		for (u32 y = 0; y < resolution; ++y) {
			for (u32 x = 0; x < resolution; ++x) {

				const u32 first = (y * (resolution + 1)) + x + baseIndex;
				const u32 second = first + resolution + 1;
				add_quad(first + 1, second + 1, second, first);
			}
		}

		return add_surface(baseIndex, indexCount);
	}


#define ADD_DISJOINTED_LINE(start, end, color)		add_debug_line(start, math::lerp(start, end, 0.33f), color);					\
													add_debug_line(math::lerp(start, end, 0.66f), end, color)

	void procedural_mesh_script::show_debug_bounds(PFF::geometry::bounds bounds) {

		const glm::vec4 color(0.0f, 1.0f, 1.0f, 1.0f);				// Cyan color
		const glm::vec3 min = bounds.origin - bounds.extents;
		const glm::vec3 max = bounds.origin + bounds.extents;
		const std::array<glm::vec3, 8> corners = {

			glm::vec3(min.x, min.y, min.z),
			glm::vec3(max.x, min.y, min.z),
			glm::vec3(min.x, max.y, min.z),
			glm::vec3(max.x, max.y, min.z),
			glm::vec3(min.x, min.y, max.z),
			glm::vec3(max.x, min.y, max.z),
			glm::vec3(min.x, max.y, max.z),
			glm::vec3(max.x, max.y, max.z)
		};

		// Bottom face
		ADD_DISJOINTED_LINE(corners[0], corners[1], color);
		ADD_DISJOINTED_LINE(corners[1], corners[3], color);
		ADD_DISJOINTED_LINE(corners[3], corners[2], color);
		ADD_DISJOINTED_LINE(corners[2], corners[0], color);

		// Top face
		ADD_DISJOINTED_LINE(corners[4], corners[5], color);
		ADD_DISJOINTED_LINE(corners[5], corners[7], color);
		ADD_DISJOINTED_LINE(corners[7], corners[6], color);
		ADD_DISJOINTED_LINE(corners[6], corners[4], color);

		// Connecting edges
		ADD_DISJOINTED_LINE(corners[0], corners[4], color);
		ADD_DISJOINTED_LINE(corners[1], corners[5], color);
		ADD_DISJOINTED_LINE(corners[2], corners[6], color);
		ADD_DISJOINTED_LINE(corners[3], corners[7], color);
	}

	void procedural_mesh_script::clear_debug_line() {		GET_RENDERER.clear_debug_line(); }

	void procedural_mesh_script::apply_mesh(const bool recalc_bounds) {

		if (recalc_bounds)
			m_mesh_asset.calc_bounds();

		GET_RENDERER.update_mesh(m_mesh_asset, m_mesh_asset.indices, m_mesh_asset.vertices);
	}










	//void procedural_mesh_script::find_mesh_intersections(const geometry::procedural_mesh_asset& mesh1, const geometry::procedural_mesh_asset& mesh2, std::vector<std::pair<u32, u32>>& intersections) {
	//	
	//	// Implement intersection finding algorithm
	//	// This is a complex operation, often involving spatial partitioning structures like octrees or BVHs
	//	// For simplicity, you might start with a brute-force approach
	//}

	//void procedural_mesh_script::cut_mesh_along_intersections(geometry::procedural_mesh_asset& mesh, const std::vector<std::pair<u32, u32>>& intersections) {

	//	// Implement mesh cutting algorithm
	//	// This involves splitting triangles and creating new vertices along intersection lines
	//}

	//void procedural_mesh_script::remove_interior_triangles(geometry::procedural_mesh_asset& mesh, const geometry::procedural_mesh_asset& subtractor) {

	//	// Implement algorithm to determine which triangles are inside the subtractor mesh
	//	// Remove these triangles from the main mesh
	//}

	//void procedural_mesh_script::merge_meshes(geometry::procedural_mesh_asset& mesh1, const geometry::procedural_mesh_asset& mesh2) {

	//	// Implement mesh merging algorithm
	//	// This involves combining vertices and triangles from both meshes
	//}
}
