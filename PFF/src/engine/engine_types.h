#pragma once


typedef struct {
	vec2 pos;
	vec2 size;
} AABB;

typedef struct {
	AABB aabb;
	vec2 velocity;
	vec2 acceleration;
} pyhsics_body;
