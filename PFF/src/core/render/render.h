#pragma once

#include <SDL.h>
#include <linmath.h>

#include "core/types.h"

typedef struct render_state {
    SDL_Window* window;
    f32 width;
    f32 height;
} Render_State;

typedef struct render_state_internal {
    u32 vao_quad;
    u32 vbo_quad;
    u32 ebo_quad;
} Render_State_Internal;

void render_init(void);
void render_begin(void);
void render_end(void);
void render_quad(vec2 pos, vec2 size, vec4 color);
