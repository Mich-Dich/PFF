#pragma once

#include "engine/geometry/mesh.h"

namespace PFF::geometry {

	struct intersection {

		std::vector<geometry::vertex>	intersection_points{};			// line of vertecies to mark 1 intersection loop (should not be open/disjointed unless mesh is broken/2D)
	};

	struct mesh_intersection {

		ref<PFF::geometry::mesh_asset>	main_mesh_asset{};				// refrence to main mesh
		ref<PFF::geometry::mesh_asset>	intersected_mesh_asset{};		// refrence to intersection mesh that was used for intersection-check

		std::vector<intersection>		intersections{};
	};

}
