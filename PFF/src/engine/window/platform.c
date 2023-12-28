
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
    CORE_ASSERT(SDL_Init(SDL_INIT_VIDEO) >= 0, "", "Could not init SDL: %s\n", SDL_GetError())

    SDL_Window* window = SDL_CreateWindow(
        "MyGame",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL
    );

    CORE_ASSERT(window, "", "Failed to init window: %s\n", SDL_GetError());
    SDL_GL_CreateContext(window);
    CORE_ASSERT(gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), "", "Failed to load GL: %s\n", SDL_GetError());

    CORE_LOG(Info, "OpenGL Loaded");
    CORE_LOG(Info, "Vendor:   %s", glGetString(GL_VENDOR));
    CORE_LOG(Info, "Renderer: %s", glGetString(GL_RENDERER));
    CORE_LOG(Info, "Version:  %s", glGetString(GL_VERSION));

    global.render.window = window;
    CORE_LOG_INIT_SUBSYS("window")
    return window;
}

//
void window_update(void) {

    SDL_GetWindowPosition(global.render.window, &global.render.pos_x, &global.render.pos_y);
    SDL_GetWindowSize(global.render.window, &global.render.width, &global.render.height);
}
