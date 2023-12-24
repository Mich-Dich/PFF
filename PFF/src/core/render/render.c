#include <glad/glad.h>
#undef APIENTRY

#include <stdio.h>

#include "core/IOCore/IOCore.h"
#include "core/global.h"
#include "render.h"
#include "core/platform.h"

static Render_State_Internal state = { 0 };


// ------------------------------------------------------------------------------------------ private funcs ------------------------------------------------------------------------------------------

void render_init_color_texture(u32* texture);
void render_init_quad(u32* vao, u32* vbo, u32* ebo);
void render_init_shaders(Render_State_Internal* state);
u32 render_shader_create(const char* path_vert, const char* path_frag);


// ========================================================================================== PUBLIC FUNCS ==========================================================================================
 
//
void render_init(void) {

    render_init_quad(&state.vao_quad, &state.vbo_quad, &state.ebo_quad);
    render_init_shaders(&state);
    render_init_color_texture(&state.texture_color);
}

//
void render_begin(void) {

    glClearColor(0.05f, 0.05f, 0.05f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

//
void render_end(void) {

    SDL_GL_SwapWindow(global.render.window);
}

//
void render_quad(vec2 pos, vec2 size, vec4 color) {

    glUseProgram(state.shader_default);

    mat4x4 model;
    mat4x4_identity(model);

    mat4x4_translate(model, pos[0], pos[1], 0);
    mat4x4_scale_aniso(model, model, size[0], size[1], 1);

    glUniformMatrix4fv(glGetUniformLocation(state.shader_default, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform4fv(glad_glGetUniformLocation(state.shader_default, "color"), 1, color);

    glBindVertexArray(state.vao_quad);

    glBindTexture(GL_TEXTURE_2D, state.texture_color);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}

// ========================================================================================== PRIVATE FUNCS ==========================================================================================

//
void render_init_color_texture(u32* texture) {

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    u8 solid_white[4] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, solid_white);

    glBindTexture(GL_TEXTURE_2D, 0);
}

//
void render_init_quad(u32* vao, u32* vbo, u32* ebo) {

    //     x,    y, z, u, v
    f32 vertices[] = {
         0.5,  0.5, 0, 0, 0,
         0.5, -0.5, 0, 0, 1,
        -0.5, -0.5, 0, 1, 1,
        -0.5,  0.5, 0, 1, 0
    };

    u32 indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glGenBuffers(1, ebo);

    glBindVertexArray(*vao);

    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // xyz
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), NULL);
    glEnableVertexAttribArray(0);

    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

//
void render_init_shaders(Render_State_Internal* state) {

    state->shader_default = render_shader_create("./shaders/default.vert", "./shaders/default.frag");

    mat4x4_ortho(state->projection, 0, global.render.width, 0, global.render.height, -2, 2);

    glUseProgram(state->shader_default);
    glUniformMatrix4fv(
        glGetUniformLocation(state->shader_default, "projection"),
        1,
        GL_FALSE,
        &state->projection[0][0]
    );
}

//
u32 render_shader_create(const char* path_vert, const char* path_frag) {

    int success;
    char log[512];

    File file_vertex = read_from_file(path_vert);
    CL_ASSERT(file_vertex.is_valid, "", "Error reading vert-shader: %s\n", path_vert);

    u32 shader_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader_vertex, 1, (const char* const*)&file_vertex, NULL);
    glCompileShader(shader_vertex);
    glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &success);

    CL_ASSERT_PLUS(success, glGetShaderInfoLog(shader_vertex, 512, NULL, log), "", "Error compiling vertex shader: %s\n", path_vert);

    File file_fragment = read_from_file(path_frag);
    CL_ASSERT(file_fragment.is_valid, "", "Error reading frag-shader: %s\n", path_vert);

    u32 shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader_fragment, 1, (const char* const*)&file_fragment, NULL);
    glCompileShader(shader_fragment);
    glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &success);
    CL_ASSERT_PLUS(success, glGetShaderInfoLog(shader_fragment, 512, NULL, log), "", "Error compiling fragment shader: %s\n", path_vert);

    u32 shader = glCreateProgram();
    glAttachShader(shader, shader_vertex);
    glAttachShader(shader, shader_fragment);
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    CL_ASSERT_PLUS(success, glGetProgramInfoLog(shader, 512, NULL, log), "", "Error linking shader: %s\n", log);

    free(file_vertex.data);
    free(file_fragment.data);

    return shader;
}