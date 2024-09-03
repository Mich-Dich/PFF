
#include "util/pffpch.h"

#include "engine/render/vulkan/vk_renderer.h"

#include "mesh.h"

namespace PFF::geometry {



	void mesh_asset::calc_bounds() {
		
		PFF_ISOLATED_PROFILER_SCOPED(1000, "calc frustum planes ", PFF::duration_precision::microseconds);

		CORE_ASSERT(surfaces.size() > 0, "", "Cannot calculate the bounds of an emty mesh_asset. Needs to have atleast one surface");

		// Fist loop over surfaces
		for (auto loop_surface : surfaces) {

			//loop the vertices of this surface, find min/max bounds
			glm::vec3 min_pos = vertices[loop_surface.startIndex].position;
			glm::vec3 max_pos = vertices[loop_surface.startIndex].position;
			for (u32 x = loop_surface.startIndex; x < (loop_surface.startIndex + loop_surface.count-1); x++) {

				min_pos = glm::min(min_pos, vertices[indices[x]].position);
				max_pos = glm::max(max_pos, vertices[indices[x]].position);
			}
			// calculate origin and extents from the min/max, use extent lenght for radius
			loop_surface.bounds.origin = (max_pos + min_pos) / 2.f;
			loop_surface.bounds.extents = (max_pos - min_pos) / 2.f;
			loop_surface.bounds.sphere_radius = glm::length(loop_surface.bounds.extents);
		}


		// calc mesh_asset bounds based on surfaces
		glm::vec3 minpos = surfaces[0].bounds.origin - surfaces[0].bounds.extents;
		glm::vec3 maxpos = surfaces[0].bounds.origin + surfaces[0].bounds.extents;
		for (size_t x = 0; x < surfaces.size(); x++) {
			minpos = glm::min(minpos, surfaces[x].bounds.origin - surfaces[x].bounds.extents);
			maxpos = glm::max(maxpos, surfaces[x].bounds.origin + surfaces[x].bounds.extents);
		}
		bounds.origin = (maxpos + minpos) / 2.f;
		bounds.extents = (maxpos - minpos) / 2.f;
		bounds.sphere_radius = glm::length(bounds.extents);
	}

	//procedural_mesh_asset::~procedural_mesh_asset() { 
	//	
	//	CORE_LOG(Error, "SHUTINGDOWN procedural_mesh_script");
	//	GET_RENDERER.cleanup_procedural_mesh(this);
	//}

	void procedural_mesh_asset::update(const std::vector<u32>& new_indices, const std::vector<vertex>& new_vertices) {
	}

	void procedural_mesh_asset::cleanup() {
	}

}
