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

SDL_Window* render_init_window(u32 width, u32 height);
void render_init_quad(u32* vao, u32* vbo, u32* ebo);
void render_init_color_texture(u32* texture);
void render_init_shaders(Render_State_Internal* state);
u32 render_shader_create(const char* path_vert, const char* path_frag);