
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

#define GET_VAR_NAME_STR(x) #x
#define INPUT_ACTION_REGISTER_NAME(var)         var.action_name = #var

static bool running = true;
static vec2 PlayerPos;




int main(int argc, char* argv[]) {

    log_init("main", "logs", "[$B$L$X$E] [$B$F: $G$E] - $B$C$E$Z", CL_THREAD_ID, CL_FALSE);

    int Testing_Naming_Macro_Var = 42;
    CL_LOG(Info, "name of var: %s", GET_VAR_NAME_STR(Testing_Naming_Macro_Var));

    time_init(60);
    config_init();
    create_window(1200, 800);
    render_init();

    input_init();



    input_action move_left = { 0 };
    INPUT_ACTION_REGISTER_NAME(move_left);
    move_left.description = "Move the player left";
    move_left.triger_when_paused = false;
    move_left.settings.trigger_flags |= INPUT_ACTION_TRIGGER_KEY_DOWN;
    move_left.settings.duration_in_sec = 0.5f;
    register_key_binding_bool("A", &move_left);

    input_action move_right = { 0 };
    INPUT_ACTION_REGISTER_NAME(move_right);
    move_right.description = "Move the player right";
    move_right.triger_when_paused = false;
    move_right.settings.trigger_flags |= INPUT_ACTION_TRIGGER_KEY_DOWN;
    move_right.settings.duration_in_sec = 0.3f;
    register_key_binding_bool("D", &move_right);

    input_action move_up_down = { 0 };
    INPUT_ACTION_REGISTER_NAME(move_up_down);
    move_up_down.description = "Move the player up";
    move_up_down.triger_when_paused = false;
    move_up_down.settings.trigger_flags |= INPUT_ACTION_TRIGGER_KEY_DOWN;
    move_up_down.settings.modefiers_flags |= INPUT_ACTION_MODEFIER_SMOOTH;
    move_up_down.settings.duration_in_sec = 0.5f;
    register_key_binding_float("W", "S", &move_up_down);
    
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
