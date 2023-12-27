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
    movement.settings.duration_in_sec = 0.5f;
    input_register_action_vec2("D", "A", "W", "S", &movement);

    // set player starting pos
    i32 window_w = get_window_width();
    i32 window_h = get_window_height();
    PlayerPos[0] = window_w * 0.5f;
    PlayerPos[1] = window_h * 0.5f;

    test_aabb = (AABB){
        .pos = {window_w * 0.5, window_h * 0.5},
        .size = {50, 50}
    };

    cursor_aabb = (AABB){ .size = {75, 75} };

    start_aabb = (AABB){ .size = {75, 75} };

    sum_aabb = (AABB){
        .pos= {test_aabb.pos[0], test_aabb.pos[1]},
        .size = {
            test_aabb.size[0] + cursor_aabb.size[0],
            test_aabb.size[1] + cursor_aabb.size[1]}
    };

	return true;
}

//
bool game_update(game* game_inst, f64 delta_time) {

    if (change_key_binding.boolean)
        input_update_action_binding_vec2(&movement, "K", "H", "U", "J");


    if (movement.vector2D[0] != 0.0f || movement.vector2D[1] != 0.0f) {

        vec2 buf = { 0 };
        vec2_scale(buf, movement.vector2D, 500 * delta_time);
        vec2_add(PlayerPos, PlayerPos, buf);
    }

    if (quit.boolean)
        QuitGame();

    i32 mousePos_x, mousePos_y;
    get_courser_pos(&mousePos_x, &mousePos_y);
    /*
    PlayerPos[0] = mousePos_x;
    PlayerPos[1] = get_window_height() - mousePos_y;*/


    // LOG(Debug, "Mouse in Window: %s    mouse [%d / %d]", bool_to_str(is_courser_in_window()), mousePos_x, mousePos_y);

	return true;
}

//
bool game_render(game* game_inst, f64 delta_time) {

    cursor_aabb.pos[0] = PlayerPos[0];
    cursor_aabb.pos[1] = PlayerPos[1];

    render_aabb((f32*)&test_aabb, WHITE);

    render_aabb((f32*)&sum_aabb, (vec4) { 1, 1, 1, 0.5 });

    AABB minkowski_difference = aabb_minkowski_difference(test_aabb, cursor_aabb);
    render_aabb((f32*)&minkowski_difference, ORANGE);

    vec2 pv;
    aabb_penetration_vector(pv, minkowski_difference);

    AABB collision_aabb = cursor_aabb;
    collision_aabb.pos[0] += pv[0];
    collision_aabb.pos[1] += pv[1];

    if (physics_test_intersect_aabb_aabb(test_aabb, cursor_aabb)) {
        render_aabb((f32*)&cursor_aabb, RED);
        render_aabb((f32*)&collision_aabb, CYAN);

        vec2_add(pv, PlayerPos, pv);
        render_line_segment(PlayerPos, pv, CYAN);
    } else {
        render_aabb((f32*)&cursor_aabb, WHITE);
    }

    render_aabb((f32*)&start_aabb, (vec4) { 1, 1, 1, 0.5 });
    render_line_segment(start_aabb.pos, PlayerPos, WHITE);

    if (physics_test_intersect_point_aabb(PlayerPos, test_aabb))
        render_quad(PlayerPos, (vec2) { 5, 5 }, RED);
    else
        render_quad(PlayerPos, (vec2) { 5, 5 }, WHITE);

	return true;
}

//
void game_on_resize(game* game_inst, u32 width, u32 height) {

	LOG(Trace, "new size [%d / %d]", width, height);
}

