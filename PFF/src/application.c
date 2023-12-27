

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
static u32 body_count = 900;

//
bool application_create(game* game_inst) {

    CL_VALIDATE(!Iintialized, return false, "Creating application", "[application_create()] called more than once");

    app_state.game_inst = game_inst;

    // INIT SubSystems
    time_init(&global.time, game_inst->app_config.target_fps);
    input_init();
    config_init();
    window_create(
        game_inst->app_config.name,
        game_inst->app_config.start_width,
        game_inst->app_config.start_hight);

    global.render.width = game_inst->app_config.start_width;
    global.render.height = game_inst->app_config.start_hight;
    SDL_ShowCursor(false);

    render_init();
    physics_init();

    //srand((unsigned int)time(NULL));     // Seed the random number generator with the current time

    // ============================================================================ TEST-ONLY ============================================================================

    
    for (u32 x = 0; x < body_count; x++) {

        size_t body_index = physics_body_create(
            (vec2) { rand_f_in_range(0, (f32)global.render.width), rand_f_in_range(0, (f32)global.render.height) },
            (vec2) { rand_f_in_range(5, 30), rand_f_in_range(5, 30) });
        pyhsics_body* loc_body = physics_body_get_data(body_index);
        loc_body->acceleration[0] = rand_f_in_range(-100, 100);
        loc_body->acceleration[1] = rand_f_in_range(-100, 100);
    }

    // ============================================================================ TEST-ONLY ============================================================================

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
            window_update();
            input_update();
            physics_update();


            // ============================================================================ TEST-ONLY <INPUT> ============================================================================

            // LOG(Debug, "Mouse in Window: %s    mouse [%d / %d]", bool_to_str(is_courser_in_window()), global.input.courser_pos_x, global.input.courser_pos_y);

            // ============================================================================ TEST-ONLY <INPUT> ============================================================================
            
            // client update routine
            CL_VALIDATE(app_state.game_inst->update(app_state.game_inst, global.time.delta), app_state.running = false, "", "Failed to update game, shutting down");


            // game render routine
            render_begin();

            // ============================================================================ TEST-ONLY <RENDER> ============================================================================

            // physics_test_intersect_point_aabb()

            
            for (u32 x = 0; x < body_count; x++) {
                pyhsics_body* loc_body = physics_body_get_data(x);
                render_quad(loc_body->aabb.pos, loc_body->aabb.size, GREEN_DARK);

                if (loc_body->aabb.pos[0] > (global.render.width / 1) || loc_body->aabb.pos[0] < 0)
                    loc_body->velocity[0] *= -1;
                if (loc_body->aabb.pos[1] > (global.render.height / 1) || loc_body->aabb.pos[1] < 0)
                    loc_body->velocity[1] *= -1;

                clamp_f(loc_body->velocity[0], -500, 500);
                clamp_f(loc_body->velocity[1], -500, 500);
            }

            // ============================================================================ TEST-ONLY <RENDER> ============================================================================

            // client render routine
            CL_VALIDATE(app_state.game_inst->render(app_state.game_inst, global.time.delta), app_state.running = false, "", "Failed to render game, shutting down");
            render_end();

            time_limit_FPS(&global.time);

        }
    }

    application_shutdown();
    return true;
}

//
bool application_shutdown() {

    // Add shutdown code here

    return true;
}


f32 get_delta_time(void) { return global.time.delta; }

u32 get_window_width(void) { return global.render.width; }

u32 get_window_height(void) { return global.render.height; }

bool is_courser_in_window(void) { return global.input.courser_in_window; }

void get_courser_pos(i32* x, i32* y) {

    *x = global.input.courser_pos_x;
    *y = global.input.courser_pos_y;
}

void QuitGame(void) { app_state.running = false; }





