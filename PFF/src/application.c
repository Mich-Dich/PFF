

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

    // TODO: Implement this
    // app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.height);

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
            input_update();

            // game update routine
            CL_VALIDATE(app_state.game_inst->update(app_state.game_inst, global.time.delta), app_state.running = false, "", "Failed to update game, shutting down");


            // game render routine
            render_begin();
            CL_VALIDATE(app_state.game_inst->render(app_state.game_inst, global.time.delta), app_state.running = false, "", "Failed to render game, shutting down");
            render_end();

            time_limit_FPS(&global.time);

        }
    }

    application_shutdown();
    return true;
}

f32 get_delta_time(void) { return global.time.delta; }

u32 get_window_width(void) { return global.render.width; }

u32 get_window_height(void) { return global.render.height; }

//
bool application_shutdown() {

    return true;
}












