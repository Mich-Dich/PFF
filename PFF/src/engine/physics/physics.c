
#include "engine/global.h"

#include "physics.h"

typedef struct {

	c_vector* body_list;		// vector of all registered physics bodys

} pyhsics_state_internal;

// ------------------------------------------------------------------------------------------ internal variables ------------------------------------------------------------------------------------------

pyhsics_state_internal internal_state;



// ------------------------------------------------------------------------------------------ private functions ------------------------------------------------------------------------------------------




// ========================================================================================== PUBLIC FUNCS ==========================================================================================

//
void physics_init(void) {

	internal_state.body_list = c_vec_new(sizeof(pyhsics_body), 1);

	CORE_LOG_INIT_SUBSYS("physics")
}

//
void physics_update(void) {

	pyhsics_body* loc_body;

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

	pyhsics_body loc_body = {
		.aabb = {
			.pos = {position[0], position[1]},
			.half_size = {size[0] * 0.5f, size[1] * 0.5f}
		},
		.velocity = {0, 0},
		.acceleration = {0, 0}
	};

	CORE_ASSERT(c_vec_push_back(internal_state.body_list, &loc_body) == c_OK, "", "Failed to create physics body");

	return internal_state.body_list->size -1;
}

//
pyhsics_body* physics_body_get_data(size_t index) {

	return c_vec_at(internal_state.body_list, index);
}

//
bool physics_test_intersect_point_aabb(vec2 point, AABB aabb) {

	vec2 loc_min, loc_max;
	aabb_min_max(aabb, loc_min, loc_max);

	return	point[0] >= loc_min[0] &&
			point[0] <= loc_max[0] &&
			point[1] >= loc_min[1] &&
			point[1] <= loc_max[1];
}

//
bool physics_test_intersect_aabb_aabb(AABB a, AABB b) {

	vec2 min, max;
	aabb_min_max(aabb_minkowski_difference(a, b), min, max);

	return (min[0] <= 0 && max[0] >= 0 && min[1] <= 0 && max[1] >= 0);
}

//
physics_hit ray_intersect_aabb(vec2 pos, vec2 magnitude, AABB aabb) {

	physics_hit hit = { 0 };
	vec2 min, max;
	aabb_min_max(aabb, min, max);

	f32 last_entry = -INFINITY;
	f32 first_exit = INFINITY;

	for (u8 i = 0; i < 2; ++i) {
		if (magnitude[i] != 0) {
			f32 t1 = (min[i] - pos[i]) / magnitude[i];
			f32 t2 = (max[i] - pos[i]) / magnitude[i];

			last_entry = fmaxf(last_entry, fminf(t1, t2));
			first_exit = fminf(first_exit, fmaxf(t1, t2));
		} else if (pos[i] <= min[i] || pos[i] >= max[i]) {
			return hit;
		}
	}

	if (first_exit > last_entry && first_exit > 0 && last_entry < 1) {
		hit.pos[0] = pos[0] + magnitude[0] * last_entry;
		hit.pos[1] = pos[1] + magnitude[1] * last_entry;

		hit.is_hit = true;
		hit.time = last_entry;
	}

	return hit;
}


// ------------------------------------------------------------------------------------------ public functions ------------------------------------------------------------------------------------------

AABB aabb_minkowski_difference(AABB a, AABB b) {

	AABB result;
	vec2_sub(result.pos, a.pos, b.pos);
	vec2_add(result.half_size, a.half_size, b.half_size);

	return result;
}

//
void aabb_penetration_vector(vec2 r, AABB aabb) {

	vec2 min, max;
	aabb_min_max(aabb, min, max);

	f32 min_dist = fabsf(min[0]);
	r[0] = min[0];
	r[1] = 0;

	if (fabsf(max[0]) < min_dist) {
		min_dist = fabsf(max[0]);
		r[0] = max[0];
	}

	if (fabsf(min[1]) < min_dist) {
		min_dist = fabsf(min[1]);
		r[0] = 0;
		r[1] = min[1];
	}

	if (fabsf(max[1]) < min_dist) {
		r[0] = 0;
		r[1] = max[1];
	}
}

// 
void aabb_min_max(AABB aabb, vec2 min, vec2 max) {

	vec2_sub(min, aabb.pos, aabb.half_size);
	vec2_add(max, aabb.pos, aabb.half_size);
}
