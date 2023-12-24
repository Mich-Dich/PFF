
#include <SDL.h>
#pragma warning(disable: 4244)
#include "linmath.h"
#pragma warning(default: 4244)
#include <glad/glad.h>
#undef APIENTRY

#include "platform.h"

#include "core/config.h"
#include "core/input.h"
#include "core/types.h"
#include "core/logger.h"
#include "core/global.h"

// 
SDL_Window* create_window(u32 window_width, u32 window_height) {

    global.render.width = (f32)window_width;
    global.render.height = (f32)window_height;

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

    CL_LOG(Trace, "OpenGL Loaded");
    CL_LOG(Trace, "Vendor:   %s", glGetString(GL_VENDOR));
    CL_LOG(Trace, "Renderer: %s", glGetString(GL_RENDERER));
    CL_LOG(Trace, "Version:  %s", glGetString(GL_VERSION));

    global.render.window = window;
    return window;
}
