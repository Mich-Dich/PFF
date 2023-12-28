#pragma once

// =============================================  physics  =============================================

// --------------------------- shapes ---------------------------

typedef struct {
	vec2 pos;
	vec2 half_size;
} AABB;

typedef struct {
	bool is_hit;
	f32 time;
	vec2 pos;
} physics_hit;

// --------------------------- material ---------------------------

typedef struct {
	bool movable;
	f32 friction;
	f32 restetution;
	f32 density;
	f32 stickiness;
	f32 gravity;
} physics_material;

// --------------------------- physics body ---------------------------

typedef struct {
	AABB aabb;
	vec2 velocity;
	vec2 acceleration;
	physics_material phy_mat;
} physics_body;

