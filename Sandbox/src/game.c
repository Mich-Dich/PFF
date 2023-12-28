#include "game.h"

static input_action movement;
static input_action quit;
static input_action change_key_binding;
static vec2 PlayerPos;
static AABB test_aabb;
static AABB cursor_aabb;
static AABB start_aabb;
static AABB sum_aabb;

static bool updated = false;

//
bool game_initalize(game* game_inst) {

    // register 2-axis input action
    INPUT_ACTION_REGISTER_NAME(quit);
    quit.description = "Quit the game";
    quit.settings.trigger_flags |= INPUT_ACTION_TRIGGER_KEY_DOWN;
    input_register_action_bool("Escape", &quit);

    // register 2-axis input action
    INPUT_ACTION_REGISTER_NAME(change_key_binding);
    change_key_binding.settings.trigger_flags |= INPUT_ACTION_TRIGGER_TAP;
    change_key_binding.settings.duration_in_sec = 0.3f;
    input_register_action_bool("B", &change_key_binding);

    // register 2-axis input action
    INPUT_ACTION_REGISTER_NAME(movement);
    movement.description = "Move the player in all directions";
    movement.settings.trigger_flags |= INPUT_ACTION_TRIGGER_KEY_DOWN;
    movement.settings.modefiers_flags |= INPUT_ACTION_MODEFIER_SMOOTH | INPUT_ACTION_MODEFIER_VEC2_NORMAL;
    movement.settings.duration_in_sec = 0.2f;
    input_register_action_vec2("D", "A", "W", "S", &movement);

    // set player starting pos
    i32 window_w = get_window_width();
    i32 window_h = get_window_height();
    PlayerPos[0] = window_w * 0.5f;
    PlayerPos[1] = window_h * 0.5f;

    test_aabb = (AABB){
        .pos = {window_w * 0.5f, window_h * 0.5f},
        .half_size = {20, 20}
    };

    cursor_aabb = (AABB){ .half_size = {20, 5} };

    start_aabb = (AABB){ .half_size = {5, 5} };

    sum_aabb = (AABB){
        .pos= {test_aabb.pos[0], test_aabb.pos[1]},
        .half_size = {
            test_aabb.half_size[0] + cursor_aabb.half_size[0],
            test_aabb.half_size[1] + cursor_aabb.half_size[1]}
    };

	return true;
}

//
bool game_update(game* game_inst, f64 delta_time) {

    if (change_key_binding.boolean) {

        vec2_copy(start_aabb.pos, PlayerPos);


        //start_aabb.pos[0] = PlayerPos[0];
        //start_aabb.pos[1] = PlayerPos[1];
    }

    /*
    if (movement.vector2D[0] != 0.0f || movement.vector2D[1] != 0.0f) {

        vec2 buf = { 0 };
        vec2_scale(buf, movement.vector2D, (f32)(500 * delta_time));
        vec2_add(PlayerPos, PlayerPos, buf);
    }*/

    if (quit.boolean)
        QuitGame();

    i32 mousePos_x, mousePos_y;
    get_courser_pos(&mousePos_x, &mousePos_y);
    
    PlayerPos[0] = (f32)mousePos_x;
    PlayerPos[1] = (f32)mousePos_y;


    // LOG(Debug, "Mouse in Window: %s    mouse [%d / %d]", bool_to_str(is_courser_in_window()), mousePos_x, mousePos_y);

	return true;
}

//
bool game_render(game* game_inst, f64 delta_time) {

    // RENDER PHYSICS INTERACTION DEMO

    vec4 faded = { 1, 1, 1, 0.3f };
    cursor_aabb.pos[0] = PlayerPos[0];
    cursor_aabb.pos[1] = PlayerPos[1];

    render_aabb(&test_aabb, WHITE);
    render_aabb(&start_aabb, faded);
    render_aabb(&cursor_aabb, WHITE);


    render_line_segment(start_aabb.pos, PlayerPos, faded);

    f32 x = sum_aabb.pos[0];
    f32 y = sum_aabb.pos[1];
    f32 size = sum_aabb.half_size[0];

    vec2 min, max;
    aabb_min_max(sum_aabb, min, max);

    vec2 magnitude;
    vec2_sub(magnitude, PlayerPos, start_aabb.pos);

    physics_hit hit = ray_intersect_aabb(start_aabb.pos, magnitude, sum_aabb);

    if (hit.is_hit) {
        AABB hit_aabb = {
            .pos= {hit.pos[0], hit.pos[1]},
            .half_size = {start_aabb.half_size[0], start_aabb.half_size[1]}
        };
        render_aabb(&hit_aabb, CYAN);
        render_quad(hit.pos, (vec2) { 5, 5 }, CYAN);
    }

	return true;
}

//
void game_on_resize(game* game_inst, u32 width, u32 height) {

	LOG(Info, "new size [%d / %d]", width, height);
}

