#include "core/pch.h"
#include "core/input.h"

#include "core/config.h"
#include "core/input.h"
#include "core/types.h"
#include "core/logger.h"
#include "core/global.h"


//
static update_key_state(const input_value type, const u8 currentState[4], Key_state* key_state, input_action* action) {

	switch (type) {

		case IV_bool: {

			if (currentState[0]) {
				if (*key_state != KS_released) {

					*key_state = KS_held;
					action->state = AS_active;
				} else {

					*key_state = KS_pressed;
					action->state= AS_transitioning;
				}
			} else {

				*key_state = KS_released;
				action->state = AS_inactive;
			}
		} break;

		case IV_float: {

		} break;

		case IV_vec2: {

			// SET THIS FIELD
			action->vector2;

			if (currentState[0]) {
				if (*key_state != KS_released) {

					*key_state = KS_held;
					action->state = AS_active;
				} else {

					*key_state = KS_pressed;
					*action_state = AS_transitioning;
				}
			} else {

				*key_state = KS_released;
				*action_state = AS_inactive;
			}
		} break;

	default:
		break;
	}
}

//
void input_init(void) {

	global.config.key_bindings = *c_vec_new(sizeof(input_mapping), 2);
}


// inital setup of key binding of a bool value
// This is a soft process, meaning it only registers this binding if the config file doesnt habe it already
void register_key_binding_bool(const char* key, const input_action* action) {

	input_mapping new_mapping = { 0 };

	// setup mapping
	SDL_Scancode scan_code = SDL_GetScancodeFromName(key);
	CL_VALIDATE(scan_code != SDL_SCANCODE_UNKNOWN, return, "scan_code: %s/ %d", "Invalid scan code when binding key: %s", key, scan_code);
	new_mapping.key_code = (u8)scan_code;
	new_mapping.override_settings = false;
	new_mapping.action = action;

	c_vec_push_back(&global.config.key_bindings, &new_mapping);
}

// inital setup of key binding of a vec2
// This is a soft process, meaning it only registers this binding if the config file doesnt habe it already
void register_key_binding_vec2(const char* x_plus, const char* x_minus, const char* y_plus, const char* y_minus, const input_action* action) {

	CL_VALIDATE(x_plus != x_minus != y_plus != y_minus, return, "", "registering the same key multiple time is not supported");

	input_mapping new_mapping = { 0 };

	// setup mapping
	SDL_Scancode scan_code_xp = SDL_GetScancodeFromName(x_plus);
	SDL_Scancode scan_code_xm = SDL_GetScancodeFromName(x_minus);
	SDL_Scancode scan_code_yp = SDL_GetScancodeFromName(y_plus);
	SDL_Scancode scan_code_ym = SDL_GetScancodeFromName(y_minus);
	CL_VALIDATE(scan_code_xp != SDL_SCANCODE_UNKNOWN && scan_code_xm != SDL_SCANCODE_UNKNOWN && scan_code_yp != SDL_SCANCODE_UNKNOWN && scan_code_ym != SDL_SCANCODE_UNKNOWN,
		return, "keys [%s && %s && %s && %s] are valid", "Invalid scan code when binding keys: [%s && %s && %s && %s]", x_plus, x_minus, y_plus, y_minus);

	new_mapping.key_codes = c_vec_new(sizeof(u8), 4);
	c_vec_push_back(new_mapping.key_codes, &scan_code_xp);
	c_vec_push_back(new_mapping.key_codes, &scan_code_xm);
	c_vec_push_back(new_mapping.key_codes, &scan_code_yp);
	c_vec_push_back(new_mapping.key_codes, &scan_code_ym);

	new_mapping.override_settings = false;
	new_mapping.action = action;
	c_vec_push_back(&global.config.key_bindings, &new_mapping);
}

//
void input_update(void) {

	const u8* keybord_state = SDL_GetKeyboardState(NULL);

	input_mapping mapping_buf = { 0 };
	u8 loc_key_codes[4];
	for (u16 i = 0; i < c_vec_size(&global.config.key_bindings); ++i) {

		input_mapping* current_mapping = c_vec_at(&global.config.key_bindings, i);
		switch (current_mapping->action->value) {
			case IV_bool: {
				
				loc_key_codes[0] = keybord_state[current_mapping->key_code];
			} break;

			case IV_float: {

				loc_key_codes[0] = keybord_state[(u8)c_vec_at(current_mapping->key_codes, 0)];
				loc_key_codes[1] = keybord_state[(u8)c_vec_at(current_mapping->key_codes, 1)];
			} break;

			case IV_vec2: {

				loc_key_codes[0] = keybord_state[(u8)c_vec_at(current_mapping->key_codes, 0)];
				loc_key_codes[1] = keybord_state[(u8)c_vec_at(current_mapping->key_codes, 1)];
				loc_key_codes[2] = keybord_state[(u8)c_vec_at(current_mapping->key_codes, 2)];
				loc_key_codes[3] = keybord_state[(u8)c_vec_at(current_mapping->key_codes, 3)];
			} break;

			default:
				break;
		}
		update_key_state(current_mapping->action->value, loc_key_codes, &current_mapping->key_state, &current_mapping->action);
	}

	/* update_key_state(keybord_state[global.config.keybinds[IK_escape]], &global.input.escape); */
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