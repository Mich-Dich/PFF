#include "core/pch.h"
#include "core/input.h"

static update_key_state(u8 currentState, Key_state* key_state) {

	if (currentState) {
		if (*key_state > 0)
			*key_state = KS_held;
		else
			*key_state = KS_pressed;
	}
	else
		*key_state = KS_released;
}
//
void update_input(void) {

	const u8* keybord_state = SDL_GetKeyboardState(NULL);

	update_key_state(keybord_state[global.config.keybinds[INPUT_KEY_LEFT]], &global.input.left);
	update_key_state(keybord_state[global.config.keybinds[INPUT_KEY_RIGHT]], &global.input.right);
	update_key_state(keybord_state[global.config.keybinds[INPUT_KEY_UP]], &global.input.up);
	update_key_state(keybord_state[global.config.keybinds[INPUT_KEY_DOWN]], &global.input.down);
	update_key_state(keybord_state[global.config.keybinds[INPUT_KEY_ESCAPE]], &global.input.escape);
}