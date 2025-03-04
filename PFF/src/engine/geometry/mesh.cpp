
#include "util/pffpch.h"

#include "engine/render/vulkan/vk_renderer.h"

#include "mesh.h"

namespace PFF::geometry {


	void mesh_asset::calc_bounds() {
		
		ASSERT(surfaces.size() > 0, "", "Cannot calculate the bounds of an emty mesh_asset. Needs to have atleast one surface");

		PFF_ISOLATED_PROFILER_SCOPED(1000, "calc frustum planes ", PFF::duration_precision::microseconds);

		glm::vec3 mesh_min_pos = vertices[0].position;
		glm::vec3 mesh_max_pos = vertices[0].position;

        // Loop over surfaces
        for (auto& loop_surface : surfaces) {
            glm::vec3 surface_min_pos = vertices[indices[loop_surface.startIndex]].position;
            glm::vec3 surface_max_pos = vertices[indices[loop_surface.startIndex]].position;

            // Loop through all vertices of this surface
            for (u32 i = 0; i < loop_surface.count; ++i) {
                u32 vertex_index = indices[loop_surface.startIndex + i];
                const glm::vec3& vertex_pos = vertices[vertex_index].position;

                surface_min_pos = glm::min(surface_min_pos, vertex_pos);
                surface_max_pos = glm::max(surface_max_pos, vertex_pos);
            }

            // Calculate surface bounds
            loop_surface.bounds_data.origin = (surface_max_pos + surface_min_pos) * 0.5f;
            loop_surface.bounds_data.extents = (surface_max_pos - surface_min_pos) * 0.5f;
            loop_surface.bounds_data.sphere_radius = glm::length(loop_surface.bounds_data.extents);

            // Update mesh bounds
            mesh_min_pos = glm::min(mesh_min_pos, surface_min_pos);
            mesh_max_pos = glm::max(mesh_max_pos, surface_max_pos);
        }

        // Calculate mesh bounds
        bounds_data.origin = (mesh_max_pos + mesh_min_pos) * 0.5f;
        bounds_data.extents = (mesh_max_pos - mesh_min_pos) * 0.5f;
        bounds_data.sphere_radius = glm::length(bounds_data.extents);
	}

	//procedural_mesh_asset::~procedural_mesh_asset() { 
	//	
	//	LOG(Error, "SHUTINGDOWN procedural_mesh_script");
	//	GET_RENDERER.cleanup_procedural_mesh(this);
	//}

	void procedural_mesh_asset::update(const std::vector<u32>& new_indices, const std::vector<vertex>& new_vertices) {
	}

	void procedural_mesh_asset::cleanup() {
	}

}
