#include "core/pch.h"
#include "core/input.h"

#include "core/config.h"
#include "core/input.h"
#include "core/types.h"
#include "core/logger.h"
#include "core/global.h"


//
static update_key_state(u8 currentState, Key_state* key_state, action_state* action_state) {

	if (currentState) {
		if (*key_state != KS_released) {

			*key_state = KS_held;
			*action_state = AS_active;
		}
		else {

			*key_state = KS_pressed;
			*action_state = AS_transitioning;
		}
	}
	else {

		*key_state = KS_released;
		*action_state = AS_inactive;
	}
}

//
void input_init(void) {

	global.config.key_bindings = *c_vec_new(sizeof(input_mapping), 2);
}


//
void register_key_binding(const char* key, const input_action* action) {

	input_mapping new_mapping = { 0 };

	// setup mapping
	SDL_Scancode scan_code = SDL_GetScancodeFromName(key);
	CL_VALIDATE(scan_code != SDL_SCANCODE_UNKNOWN, return, "scan_code: %s/ %d", "Invalid scan code when binding key: %s\n", key, scan_code);
	new_mapping.key_code = (u8)scan_code;
	new_mapping.override_settings = false;
	new_mapping.action = action;

	c_vec_push_back(&global.config.key_bindings, &new_mapping);
}


//
void input_update(void) {

	const u8* keybord_state = SDL_GetKeyboardState(NULL);

	input_mapping mapping_buf = { 0 };
	for (u16 i = 0; i < c_vec_size(&global.config.key_bindings); ++i) {

		input_mapping* current_mapping = c_vec_at(&global.config.key_bindings, i);
		update_key_state(keybord_state[current_mapping->key_code], &current_mapping->key_state, &current_mapping->action->state);
	}

	/*
	update_key_state(keybord_state[global.config.keybinds[IK_left]], &global.input.left);
	update_key_state(keybord_state[global.config.keybinds[IK_right]], &global.input.right);
	update_key_state(keybord_state[global.config.keybinds[IK_down]], &global.input.down);
	update_key_state(keybord_state[global.config.keybinds[IK_escape]], &global.input.escape);*/
}












//
void change_key_binding_Settings(input_mapping mapping, input_action_settings settings) {

	mapping.override_settings = true;
	mapping.settings = settings;
}

//
void Get_Input_Action_State(const char* key, input_action action) {

	const u8* keybord_state = SDL_GetKeyboardState(NULL);

	input_mapping mapping_buf = { 0 };
	//mapping_buf = global.config.key_bindings.array[0];

	//update_key_state(keybord_state[global.config.keybinds[IK_up]], &global.input.up);
}