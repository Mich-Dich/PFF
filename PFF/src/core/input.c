#include "core/pch.h"
#include "core/input.h"

#include "core/config.h"
#include "core/input.h"
#include "core/types.h"
#include "core/logger.h"
#include "core/global.h"


//
static update_key_state(u8 currentState, Key_state* key_state) {

	if (currentState) {
		if (*key_state != KS_released)
			*key_state = KS_held;
		else
			*key_state = KS_pressed;
	}
	else
		*key_state = KS_released;
}

//
void input_init(void) {

	global.config.key_bindings = *darray_create(input_mapping);
}

//
void register_key_binding(const char* key, const input_action* action) {

	input_mapping new_mapping = { 0 };

	// setup mapping
	SDL_Scancode scan_code = SDL_GetScancodeFromName(key);
	CL_VALIDATE(scan_code != SDL_SCANCODE_UNKNOWN, return, "", "Invalid scan code when binding key: %s\n", key);
	new_mapping.key_code = (u8)scan_code;
	new_mapping.override_settings = false;
	memcpy(&new_mapping.action, action, sizeof(input_action));

	darray_push(&global.config.key_bindings, &new_mapping);
}

//
void change_key_binding_Settings(input_mapping mapping, input_action_settings settings) {

	mapping.override_settings = true;
	mapping.settings = settings;
}

//
void update_input(void) {

	const u8* keybord_state = SDL_GetKeyboardState(NULL);



	update_key_state(keybord_state[global.config.keybinds[IK_left]], &global.input.left);
	update_key_state(keybord_state[global.config.keybinds[IK_right]], &global.input.right);
	update_key_state(keybord_state[global.config.keybinds[IK_up]], &global.input.up);
	update_key_state(keybord_state[global.config.keybinds[IK_down]], &global.input.down);
	update_key_state(keybord_state[global.config.keybinds[IK_escape]], &global.input.escape);
}

