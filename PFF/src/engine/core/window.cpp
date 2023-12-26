#include "window.h"
#include "engine/core/Logger.h"

#include "engine/Events/ApplicationEvent.h"

#include <glad/glad.h>
#include <iostream>


#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace PFF {


    Window::Window(const WindowAttributes& attributes) {

        CORE_LOG(Trace, "Window Constructor: " << attributes.Title);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        CORE_ASSERT(SDL_Init(SDL_INIT_VIDEO) >= 0, "", "Could not init SDL: " << SDL_GetError());

        m_Window = SDL_CreateWindow(
            attributes.Title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            attributes.Width,
            attributes.Height,
            SDL_WINDOW_OPENGL
        );

        CORE_ASSERT(m_Window, "", "Failed to init window: %s" << SDL_GetError());
        SDL_GL_CreateContext(m_Window);
        CORE_ASSERT(gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), "", "Failed to load GL: " << SDL_GetError());

        CORE_LOG(Info, "OpenGL Loaded");
        CORE_LOG(Info, "Vendor:   %s" << glGetString(GL_VENDOR));
        CORE_LOG(Info, "Renderer: %s" << glGetString(GL_RENDERER));
        CORE_LOG(Info, "Version:  %s" << glGetString(GL_VERSION));
    }

    //
    Window::~Window() {}

    //
    void Window::Poll_for_Events() {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            switch (event.type) {

                case SDL_QUIT: {

                    WindowCloseEvent event();

                } break;

                case SDL_WINDOWEVENT_CLOSE: {

                

                } break;

                default:
                    break;
            }
        }
    }
}