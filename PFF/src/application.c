

#include "engine/config.h"
#include "engine/global.h"
#include "engine/window/platform.h"
#include "engine/render/render.h"
#include "engine/physics/physics.h"

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
static u32 body_count = 100;

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

    global.render.width = game_inst->app_config.start_width;
    global.render.height = game_inst->app_config.start_hight;

    render_init();
    physics_init();

    // ===================================================== TEST-ONLY =====================================================

    for (u32 x = 0; x < body_count; x++) {

        size_t body_index = physics_body_create(
            (vec2) { rand() % (i32)global.render.width, rand() % (i32)global.render.width },
            (vec2) { rand() % 100, rand() % 100 });
        body* loc_body = physics_body_get_data(body_index);
        loc_body->acceleration[0] = rand_in_range(-100, 100);
        loc_body->acceleration[1] = rand_in_range(-100, 100);
    }

    // ===================================================== TEST-ONLY =====================================================

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
            physics_update();

            // game update routine
            CL_VALIDATE(app_state.game_inst->update(app_state.game_inst, global.time.delta), app_state.running = false, "", "Failed to update game, shutting down");


            // game render routine
            render_begin();

            for (u32 x = 0; x < body_count; x++) {
                body* loc_body = physics_body_get_data(x);
                render_quad(loc_body->aabb.pos, loc_body->aabb.size, (vec4) { 1, 0, 1, 1 });

                if (loc_body->aabb.pos[0] > (global.render.width / 1) || loc_body->aabb.pos[0] < 0)
                    loc_body->velocity[0] *= -1;
                if (loc_body->aabb.pos[1] > (global.render.height / 1) || loc_body->aabb.pos[1] < 0)
                    loc_body->velocity[1] *= -1;

                clamp_f(loc_body->velocity[0], -500, 500);
                clamp_f(loc_body->velocity[1], -500, 500);
            }

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












