#pragma once

#include "pch.h"
#include "engine/engine_types.h"

static physics_material template = {
	.movable = true,
	.friction = 0.2f,
	.restetution = 0.5f,
	.density = 10.0f,
	.stickiness = 0.5f,
	.gravity = 9.81f,
};

void physics_init(void);
void physics_update(void);

PFF_API handle physics_body_create(vec2 position, vec2 size);
PFF_API physics_body* physics_body_get_data(handle index);
PFF_API bool physics_test_intersect_point_aabb(vec2 point, AABB aabb);
PFF_API bool physics_test_intersect_aabb_aabb(AABB a, AABB b);

PFF_API bool physics_set_velocity_aabb(handle target, vec2 velocity);
PFF_API bool physics_add_velocity_aabb(handle target, vec2 velocity);

// TODO: make internal
PFF_API AABB aabb_minkowski_difference(AABB a, AABB b);
PFF_API void aabb_penetration_vector(vec2 r, AABB aabb);


PFF_API void aabb_min_max(AABB aabb, vec2 min, vec2 max);
PFF_API physics_hit ray_intersect_aabb(vec2 position, vec2 magnitude, AABB aabb);
