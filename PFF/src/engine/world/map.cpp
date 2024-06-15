
#include "util/pffpch.h"

#include <entt.hpp>
#include "components.h"

#include "map.h"

namespace PFF {


	map::map() {

		entt::entity entity = m_registry.create();

	}

	map::~map() {

		entt::entity entity = m_registry.create();

	}

	// ======================================================================================================================================
	// FUNCTIONS
	// ======================================================================================================================================

	void map::on_runtime_start() { }

	void map::on_runtime_stop() { }

	void map::on_simulation_start() { }

	void map::on_simulation_stop() { }

}
