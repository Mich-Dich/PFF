#pragma once


typedef struct {
	vec2 pos;
	vec2 half_size;
} AABB;

typedef struct {
	AABB aabb;
	vec2 velocity;
	vec2 acceleration;
} pyhsics_body;

typedef struct {
	bool is_hit;
	f32 time;
	vec2 pos;
} physics_hit;

