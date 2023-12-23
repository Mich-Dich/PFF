
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#undef APIENTRY
#include <stdio.h>

#include "core/pch.h"


int main(int argc, char* argv[]) {

    log_init("main", "logs", "[$B$L$E] [$B$F: $G$E] - $B$C$E$Z", CL_THREAD_ID, CL_FALSE);

    create_window(600, 400);
    render_init();

    bool running = true;
    while (running) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            default:
                break;
            }
        }

        render_begin();

        render_quad(
            (vec2) { global.render.width * 0.5f, global.render.height * 0.5f },
            (vec2) { 50, 50 },
            (vec4) { 0.4f, 1.0f, 0.7f, 1 });

        render_end();
    }

    return 0;
}
