#pragma once

#include "util/pffpch.h"

#include "engine/geometry/mesh.h"

namespace PFF {

	class PFF_API procedural_mesh_script {
	public:

		virtual ~procedural_mesh_script() {}

	protected:
		virtual void on_create() {}
		virtual void on_destroy() {}

		virtual void on_update(f32 delta_time) {}
		virtual void on_rebuild() {}
		virtual void on_collision_overlap() {}

	private:

		geometry::mesh_asset	m_mesh_asset;

	};

}
