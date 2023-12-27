#pragma once

#include "pch.h"

typedef struct {
	vec2 pos;
	vec2 size;
} AABB;

typedef struct {
	AABB aabb;
	vec2 velocity;
	vec2 acceleration;
} body;

void physics_init(void);
void physics_update(void);
size_t physics_body_create(vec2 position, vec2 size);
body* physics_body_get_data(size_t index);
