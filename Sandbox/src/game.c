#include "game.h"

static input_action movement;
static vec2 PlayerPos;

//
bool game_initalize(game* game_inst) {

    // register 2-axis input action
    INPUT_ACTION_REGISTER_NAME(movement);
    movement.description = "Move the player in all directions";
    movement.settings.trigger_flags |= INPUT_ACTION_TRIGGER_KEY_DOWN;
    movement.settings.modefiers_flags |= INPUT_ACTION_MODEFIER_SMOOTH | INPUT_ACTION_MODEFIER_VEC2_NORMAL;
    movement.settings.duration_in_sec = 0.3f;
    register_key_binding_vec2("D", "A", "W", "S", &movement);

    // set player starting pos
    PlayerPos[0] = get_window_width() * 0.5f;
    PlayerPos[1] = get_window_height() * 0.5f;

	return true;
}

//
bool game_update(game* game_inst, f64 delta_time) {

    if (movement.vector2D[0] != 0.0f || movement.vector2D[1] != 0.0f) {

        vec2 buf = { 0 };
        vec2_scale(buf, movement.vector2D, 500 * delta_time);
        vec2_add(PlayerPos, PlayerPos, buf);
    }
	return true;
}

//
bool game_render(game* game_inst, f64 delta_time) {

    render_quad(PlayerPos, (vec2) { 50, 50 }, (vec4) { 0.4f, 1.0f, 0.7f, 1 });
	return true;
}

//
void game_on_resize(game* game_inst, u32 width, u32 height) {

	LOG(Trace, "new size [%d / %d]", width, height);
}

