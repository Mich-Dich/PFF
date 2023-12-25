
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

static void input_handle(void) {

    // Custom move_up handeling


    // old version
    if (global.input.left == KS_pressed || global.input.left == KS_held)
        PlayerPos[0] -= 500 * global.time.delta;
    if (global.input.right == KS_pressed || global.input.right == KS_held)
        PlayerPos[0] += 500 * global.time.delta;
    if (global.input.down == KS_pressed || global.input.down == KS_held)
        PlayerPos[1] -= 500 * global.time.delta;

    if (global.input.escape == KS_pressed || global.input.escape== KS_held)
        running = false;
}


int main(int argc, char* argv[]) {

    log_init("main", "logs", "[$B$L$X$E] [$B$F: $G$E] - $B$C$E$Z", CL_THREAD_ID, CL_FALSE);

    int Testing_Naming_Macro_Var = 42;
    CL_LOG(Info, "name of var: %s", GET_VAR_NAME_STR(Testing_Naming_Macro_Var));

    time_init(60);
    config_init();
    create_window(600, 400);
    render_init();

    input_init();

    input_action move_up;
    INPUT_ACTION_REGISTER_NAME(move_up);
    move_up.description = "Move the player up";
    move_up.state = KS_released;
    move_up.triger_when_paused = false;
    move_up.value = IV_vec2;
    move_up.settings.duration = 0.5f;
    move_up.settings.trigger = IAT_key_down;
    move_up.settings.modefiers = IAM_none;
    register_key_binding("W", &move_up);

    input_action move_down;
    INPUT_ACTION_REGISTER_NAME(move_down);
    move_down.description = "Move the player up";
    move_down.state = KS_released;
    move_down.triger_when_paused = false;
    move_down.value = IV_vec2;
    move_down.settings.duration = 0.5f;
    move_down.settings.trigger = IAT_key_down;
    move_down.settings.modefiers = IAM_none;
    register_key_binding("S", &move_down);

    input_action move_left;
    INPUT_ACTION_REGISTER_NAME(move_left);
    move_left.description = "Move the player up";
    move_left.state = KS_released;
    move_left.triger_when_paused = false;
    move_left.value = IV_vec2;
    move_left.settings.duration = 0.5f;
    move_left.settings.trigger = IAT_key_down;
    move_left.settings.modefiers = IAM_none;
    register_key_binding("A", &move_left);

    input_action move_right;
    INPUT_ACTION_REGISTER_NAME(move_right);
    move_right.description = "Move the player up";
    move_right.state = KS_released;
    move_right.triger_when_paused = false;
    move_right.value = IV_vec2;
    move_right.settings.duration = 0.5f;
    move_right.settings.trigger = IAT_key_down;
    move_right.settings.modefiers = IAM_none;
    register_key_binding("D", &move_right);

    /*
    CL_LOG(Warn, "size of input bindings [size: %llu]", global.config.key_bindings.size);


    DArray Type_test = *darray_create(u16);
    u16 var = 42;
    for (u16 x = 0; x < 32; x++) {
        var = (u16)rand();
        darray_push(&Type_test, &var);
    }

    u16 dest;
    for (u16 x = 0; x < 32; x++) {

        u64 addr = (u64)Type_test.array;
        addr += ((Type_test.size - 1) * Type_test.stride);
        memcpy(&dest, (void*)addr, Type_test.stride);

        CL_LOG(Trace, "Element: %d", dest);
    }*/


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

        input_update();


        if (move_up.state != AS_inactive)
            PlayerPos[1] += 500 * global.time.delta;
        if (move_down.state != AS_inactive)
            PlayerPos[1] -= 500 * global.time.delta;
        if (move_left.state != AS_inactive)
            PlayerPos[0] -= 500 * global.time.delta;
        if (move_right.state != AS_inactive)
            PlayerPos[0] += 500 * global.time.delta;

        input_handle();



        render_begin();

        render_quad(PlayerPos, (vec2) { 50, 50 }, (vec4) { 0.4f, 1.0f, 0.7f, 1 });

        render_end();

        time_update_late();
    }
     
    return 0;
}
