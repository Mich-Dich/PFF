#pragma once

#include "pch.h"
#include "engine/engine_types.h"

void physics_init(void);
void physics_update(void);

PFF_API size_t physics_body_create(vec2 position, vec2 size);
PFF_API pyhsics_body* physics_body_get_data(size_t index);
PFF_API bool physics_test_intersect_point_aabb(vec2 point, AABB aabb);
PFF_API bool physics_test_intersect_aabb_aabb(AABB a, AABB b);

// TODO: make internal
PFF_API AABB aabb_minkowski_difference(AABB a, AABB b);
PFF_API void aabb_penetration_vector(vec2 r, AABB aabb);
