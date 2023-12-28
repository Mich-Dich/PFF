
#include <glad/glad.h>
#undef APIENTRY
#include <SDL.h>

#include "engine/config.h"
#include "engine/input.h"
#include "engine/global.h"

#include "platform.h"

// 
SDL_Window* window_create(const char* title, u32 window_width, u32 window_height) {

    global.render.width = window_width;
    global.render.height = window_height;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    CL_ASSERT(SDL_Init(SDL_INIT_VIDEO) >= 0, "", "Could not init SDL: %s\n", SDL_GetError())

    SDL_Window* window = SDL_CreateWindow(
        "MyGame",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL
    );

    CL_ASSERT(window, "", "Failed to init window: %s\n", SDL_GetError());
    SDL_GL_CreateContext(window);
    CL_ASSERT(gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), "", "Failed to load GL: %s\n", SDL_GetError());

    LOG(Info, "OpenGL Loaded");
    LOG(Info, "Vendor:   %s", glGetString(GL_VENDOR));
    LOG(Info, "Renderer: %s", glGetString(GL_RENDERER));
    LOG(Info, "Version:  %s", glGetString(GL_VERSION));

    global.render.window = window;
    return window;
}

//
void window_update(void) {

    SDL_GetWindowPosition(global.render.window, &global.render.pos_x, &global.render.pos_y);
    SDL_GetWindowSize(global.render.window, &global.render.width, &global.render.height);
}

void window_shutdown(void) {

    SDL_Quit();
}