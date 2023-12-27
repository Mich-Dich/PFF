#pragma once

#include "pch.h"
#include "engine/engine_types.h"

struct SDL_Window;


typedef struct {

} pyhsics_material;


typedef struct {
    struct SDL_Window* window;
    u32 width;
    u32 height;
    u32 pos_x;
    u32 pos_y;
} Render_State;

typedef struct {
    u32 vao_quad;
    u32 vbo_quad;
    u32 ebo_quad;
    u32 vao_line;
    u32 vbo_line;
    u32 shader_default;
    u32 texture_color;
    mat4x4 projection;
} Render_State_Internal;

// engine funcs
void render_init(void);
void render_begin(void);
void render_end(void);

// client usable funcs
PFF_API void render_quad(vec2 pos, vec2 size, vec4 color);

PFF_API void render_line_segment(vec2 start, vec2 end, vec4 color);

PFF_API void render_quad_line(vec2 pos, vec2 size, vec4 color);

PFF_API void render_aabb(AABB* aabb, vec4 color);
