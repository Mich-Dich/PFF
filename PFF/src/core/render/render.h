#pragma once

#include <SDL.h>
#pragma warning(disable: 4244)
#include "linmath.h"
#pragma warning(default: 4244)

#include "core/types.h"

typedef struct {
    SDL_Window* window;
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

void render_init(void);
void render_begin(void);
void render_end(void);
void render_quad(vec2 pos, vec2 size, vec4 color);
