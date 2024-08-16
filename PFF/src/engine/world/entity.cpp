#include "util/pffpch.h"
#include "entity.h"


namespace PFF {

	void entity::add_mesh_component(mesh_component mesh_comp) {

		if (has_component<mesh_component>()) {		// already has mesh_component  => add new entity and make child of this entity

		} else {		// does not have mesh component  => add component to root

		}

	}

}
