#pragma once

#include "pch.h"

struct SDL_Window;


typedef struct {
    struct SDL_Window* window;
    f32 width;
    f32 height;
} Render_State;

typedef struct {
    u32 vao_quad;
    u32 vbo_quad;
    u32 ebo_quad;
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
