
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#undef APIENTRY
#include <stdio.h>

#include "core/input.h"
#include "core/pch.h"

#include "core/config.h"
#include "core/input.h"
#include "core/types.h"
#include "core/logger.h"
#include "core/global.h"
#include "core/utils/platform.h"

#include <windows.h>


static bool running = true;

static vec2 PlayerPos;

static void input_handle(void) {

    if (global.input.left == KS_pressed || global.input.left == KS_held)
        PlayerPos[0] -= 500 * global.time.delta;
    if (global.input.right == KS_pressed || global.input.right == KS_held)
        PlayerPos[0] += 500 * global.time.delta;
    if (global.input.up == KS_pressed || global.input.up == KS_held)
        PlayerPos[1] += 500 * global.time.delta;
    if (global.input.down == KS_pressed || global.input.down == KS_held)
        PlayerPos[1] -= 500 * global.time.delta;

    if (global.input.escape == KS_pressed || global.input.escape== KS_held)
        running = false;
}


int main(int argc, char* argv[]) {

    log_init("main", "logs", "[$B$L$E] [$B$F: $G$E] - $B$C$E$Z", CL_THREAD_ID, CL_FALSE);

    time_init(60);
    config_init();
    create_window(600, 400);
    render_init();

    PlayerPos[0] = global.render.width * 0.5f;
    PlayerPos[1] = global.render.height * 0.5f;

    while (running) {

        time_update();

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

        update_input();
        input_handle();

        render_begin();

        render_quad(PlayerPos, (vec2) { 50, 50 }, (vec4) { 0.4f, 1.0f, 0.7f, 1 });

        render_end();

        time_update_late();
    }
     
    return 0;
}
