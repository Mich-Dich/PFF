

#include "engine/config.h"
#include "engine/global.h"
#include "engine/window/platform.h"
#include "engine/render/render.h"

#include "game_types.h"
#include "application.h"

#include <stdio.h>
#include <SDL.h>


typedef struct {
    game* game_inst;
    bool running;
    bool suspended;
    //platform_state platform;
    u16 width;
    u16 height;
    f64 last_time;
} application_state;

static bool Iintialized = false;
static application_state app_state;

//
bool application_create(game* game_inst) {

    CL_VALIDATE(!Iintialized, return false, "Creating application", "[application_create()] called more than once");

    app_state.game_inst = game_inst;

    // INIT SubSystems
    time_init(&global.time, game_inst->app_config.target_fps);
    input_init();
    config_init();
    create_window(
        game_inst->app_config.name,
        game_inst->app_config.start_width,
        game_inst->app_config.start_hight);

    render_init();

    app_state.running = true;
    app_state.suspended = false;

    CL_ASSERT(app_state.game_inst->initalize(app_state.game_inst), "Initializing game", "Failed to Initialize game");

    app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.height);

    Iintialized = true;
    return true;
}

//
bool application_run() {

    // Game loop
    while (app_state.running) {

        // TODO: move poll_events into function
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            case SDL_QUIT:
                app_state.running = false;
                break;

            default:
                break;
            }
        }

        // update game
        if (!app_state.suspended) {

            time_update(&global.time);

            // game update routine
            CL_VALIDATE(app_state.game_inst->update(app_state.game_inst, (f64)0.2), app_state.running = false, "", "Failed to update game, shutting down");

            // game render routine
            CL_VALIDATE(app_state.game_inst->render(app_state.game_inst, (f64)0.2), app_state.running = false, "", "Failed to render game, shutting down");


            LOG(Debug, "\033[Am FPS: %d        ", global.time.fps);
            time_limit_FPS(&global.time);

        }
    }

    application_shutdown();
    return true;
}

//
bool application_shutdown() {

    return true;
}














/*
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#undef APIENTRY
#include <stdio.h>

#include "core/input.h"
#include "core/pch.h"
#include "core/container/darray.h"

#include "core/config.h"
#include "core/input.h"
#include "core/types.h"
#include "core/logger.h"
#include "core/global.h"
#include "core/utils/platform.h"

#include <windows.h>


static bool running = true;
static vec2 PlayerPos;




int main(int argc, char* argv[]) {

    log_init("main", "logs", "[$B$L$X$E] [$B$F: $G$E] - $B$C$E$Z", CL_THREAD_ID, CL_FALSE);

    time_init(60);
    config_init();
    create_window(1200, 800);
    render_init();
    input_init();


       
    input_action movement = { 0 };
    INPUT_ACTION_REGISTER_NAME(movement);
    movement.description = "Move the player in all directions";
    movement.settings.trigger_flags |= INPUT_ACTION_TRIGGER_KEY_DOWN;
    movement.settings.modefiers_flags |= INPUT_ACTION_MODEFIER_SMOOTH;
    movement.settings.duration_in_sec = 0.3f;
    register_key_binding_vec2("D", "A", "W", "S", &movement);


    PlayerPos[0] = global.render.width * 0.5f;
    PlayerPos[1] = global.render.height * 0.5f;

    CL_LOG(Debug, "  ");
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

        time_update();

        input_update();
        if (movement.vector2D[0] != 0.0f || movement.vector2D[1] != 0.0f) {

            vec2 buf = { 0 };
            vec2_scale(buf, movement.vector2D, 500 * global.time.delta);
            vec2_add(PlayerPos, PlayerPos, buf);
        }


        render_begin();

        render_quad(PlayerPos, (vec2) { 50, 50 }, (vec4) { 0.4f, 1.0f, 0.7f, 1 });

        render_end();

        time_update_late();
    }
     
    return 0;
}
*/
