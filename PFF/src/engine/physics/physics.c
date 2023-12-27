
#include "engine/global.h"

#include "physics.h"

typedef struct {

	c_vector* body_list;		// vector of all registered physics bodys

} pyhsics_state_internal;

// ------------------------------------------------------------------------------------------ internal variables ------------------------------------------------------------------------------------------

pyhsics_state_internal internal_state;



// ------------------------------------------------------------------------------------------ public functions ------------------------------------------------------------------------------------------




// ========================================================================================== PUBLIC FUNCS ==========================================================================================

//
void physics_init(void) {

	internal_state.body_list = c_vec_new(sizeof(body), 1);
}

//
void physics_update(void) {

	body* loc_body;

	for (u32 x = 0; x < internal_state.body_list->size; x++) {

		loc_body = c_vec_at(internal_state.body_list, x);
		loc_body->velocity[0] += loc_body->acceleration[0] * global.time.delta;
		loc_body->velocity[1] += loc_body->acceleration[1] * global.time.delta;
		loc_body->aabb.pos[0] += loc_body->velocity[0] * global.time.delta;
		loc_body->aabb.pos[1] += loc_body->velocity[1] * global.time.delta;
	}

}

//
size_t physics_body_create(vec2 position, vec2 size) {

	body loc_body = {
		.aabb = {
			.pos = {position[0], position[1]},
			.size = {size[0] * 0.5f, size[1] * 0.5f}
		},
		.velocity = {0, 0},
		.acceleration = {0, 0}
	};

	CL_ASSERT(c_vec_push_back(internal_state.body_list, &loc_body) == c_OK, "", "Failed to create physics body");

	return internal_state.body_list->size -1;
}

//
body* physics_body_get_data(size_t index) {

	return c_vec_at(internal_state.body_list, index);
}

// ------------------------------------------------------------------------------------------ public functions ------------------------------------------------------------------------------------------
