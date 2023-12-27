
#include <SDL.h>

#include "engine/config.h"
#include "engine/core.h"
#include "engine/global.h"

#include "input.h"

#define UPDATE_INPUT_FLOAT(x)			{	loc_key_code = c_vec_at(current_mapping->key_codes, 0);					\
											if (x keybord_state[*loc_key_code])										\
												target += 1.0f;														\
																													\
											loc_key_code = c_vec_at(current_mapping->key_codes, 1);					\
											if (x keybord_state[*loc_key_code])										\
												target -= 1.0f;													}

#define UPDATE_INPUT_VECTOR(x)			{	loc_key_code = c_vec_at(current_mapping->key_codes, 0);					\
											if (x keybord_state[*loc_key_code])										\
												target[0] += 1.0f;													\
																													\
											loc_key_code = c_vec_at(current_mapping->key_codes, 1);					\
											if (x keybord_state[*loc_key_code])										\
												target[0] -= 1.0f;													\
																													\
											loc_key_code = c_vec_at(current_mapping->key_codes, 2);					\
											if (x keybord_state[*loc_key_code])										\
												target[1] += 1.0f;													\
																													\
											loc_key_code = c_vec_at(current_mapping->key_codes, 3);					\
											if (x keybord_state[*loc_key_code])										\
												target[1] -= 1.0f;												}


#define BIT_ENCODE_KEYBORD_STATE(x)			if (keybord_state[*(u8*)c_vec_at(current_mapping->key_codes, x)])		\
												current_mapping->buffer |= BIT(x)

#define SET_TIMESTAMP_TO_PAST				current_mapping->time_stamp = Subtract_Seconds_From_TimeStamp(current_mapping->time_stamp, (current_mapping->settings.duration_in_sec * 2))

#define CHANGE_VAR_BY_DELTA(index, sign)	target##index = buffer##index sign global.time.delta / current_mapping->settings.duration_in_sec

// ------------------------------------------------------------------------------------------ privete funcs ------------------------------------------------------------------------------------------

static inline void __update_key_state(input_mapping* mapping, const bool is_pressed);


// ------------------------------------------------------------------------------------------ main functions ------------------------------------------------------------------------------------------
//
void input_init(void) {

	global.config.key_bindings = *c_vec_new(sizeof(input_mapping), 1);
}

//
void change_key_binding_Settings(input_mapping mapping, input_action_settings settings) {

	mapping.override_settings = true;
	mapping.settings = settings;
}


// inital setup of key binding of a bool value
// This is a soft process, meaning it only registers this binding if the config file doesnt habe it already
void register_key_binding_bool(const char* key, input_action* action) {

	SDL_Scancode scan_code = SDL_GetScancodeFromName(key);
	CL_VALIDATE(scan_code != SDL_SCANCODE_UNKNOWN, return, "scan_code: %s/ %d", "Invalid scan code when binding key: %s", key, scan_code);

	action->value = IV_bool;
	input_mapping new_mapping = { 0 };
	new_mapping.key_codes = c_vec_new(sizeof(u8), 1);
	c_vec_push_back(new_mapping.key_codes, &scan_code);

	new_mapping.override_settings = false;
	new_mapping.settings = action->settings;
	new_mapping.action = action;
	c_vec_push_back(&global.config.key_bindings, &new_mapping);
}

// inital setup of key binding of a float
// This is a soft process, meaning it only registers this binding if the config file doesnt habe it already
void register_key_binding_float(const char* plus, const char* minus, input_action* action) {

	CL_VALIDATE(plus != minus, return, "", "registering the same key multiple time is not supported");

	SDL_Scancode scan_code_p = SDL_GetScancodeFromName(plus);
	SDL_Scancode scan_code_m = SDL_GetScancodeFromName(minus);
	CL_VALIDATE(scan_code_p != SDL_SCANCODE_UNKNOWN && scan_code_m != SDL_SCANCODE_UNKNOWN,
		return, "keys [%s %s] are valid", "Invalid scan code when binding keys: [%s %s]", plus, minus);

	action->value = IV_float;
	input_mapping new_mapping = { 0 };
	new_mapping.key_codes = c_vec_new(sizeof(u8), 2);
	c_vec_push_back(new_mapping.key_codes, &scan_code_p);
	c_vec_push_back(new_mapping.key_codes, &scan_code_m);

	new_mapping.override_settings = false;
	new_mapping.settings = action->settings;
	new_mapping.action = action;
	c_vec_push_back(&global.config.key_bindings, &new_mapping);
}

// inital setup of key binding of a vec2
// This is a soft process, meaning it only registers this binding if the config file doesnt habe it already
void register_key_binding_vec2(const char* x_plus, const char* x_minus, const char* y_plus, const char* y_minus, input_action* action) {

	CL_VALIDATE((x_plus != x_minus) && (y_plus != y_minus) && (x_plus != y_plus) && (x_minus != y_minus), return, "", "registering the same key multiple time is not supported");

	SDL_Scancode scan_code_xp = SDL_GetScancodeFromName(x_plus);
	SDL_Scancode scan_code_xm = SDL_GetScancodeFromName(x_minus);
	SDL_Scancode scan_code_yp = SDL_GetScancodeFromName(y_plus);
	SDL_Scancode scan_code_ym = SDL_GetScancodeFromName(y_minus);
	CL_VALIDATE(scan_code_xp != SDL_SCANCODE_UNKNOWN && scan_code_xm != SDL_SCANCODE_UNKNOWN && scan_code_yp != SDL_SCANCODE_UNKNOWN && scan_code_ym != SDL_SCANCODE_UNKNOWN,
		return, "keys [%s %s %s %s] are valid", "Invalid scan code when binding keys: [%s %s %s %s]", x_plus, x_minus, y_plus, y_minus);

	action->value = IV_vec2;
	input_mapping new_mapping = { 0 };
	new_mapping.key_codes = c_vec_new(sizeof(u8), 4);
	c_vec_push_back(new_mapping.key_codes, &scan_code_xp);
	c_vec_push_back(new_mapping.key_codes, &scan_code_xm);
	c_vec_push_back(new_mapping.key_codes, &scan_code_yp);
	c_vec_push_back(new_mapping.key_codes, &scan_code_ym);

	new_mapping.override_settings = false;
	new_mapping.settings = action->settings;
	new_mapping.action = action;
	c_vec_push_back(&global.config.key_bindings, &new_mapping);
}

//
void input_update(void) {

	/*
	#define INPUT_ACTION_TRIGGER_KEY_DOWN		BIT(0)
	#define INPUT_ACTION_TRIGGER_KEY_UP			BIT(1)
	#define INPUT_ACTION_TRIGGER_HOLD			BIT(2)
	#define INPUT_ACTION_TRIGGER_TAP			BIT(3)

	#define INPUT_ACTION_MODEFIER_NEGATE		BIT(0)
	#define INPUT_ACTION_MODEFIER_VEC2_NORMAL	BIT(1)
	#define INPUT_ACTION_MODEFIER_SMOOTH		BIT(2)
	*/

	const u8* keybord_state = SDL_GetKeyboardState(NULL);

	input_mapping* current_mapping = { 0 };
	u8 loc_key_state = 0;
	u8* loc_key_code = 0;
	for (u16 x = 0; x < c_vec_size(&global.config.key_bindings); ++x) {

		current_mapping = c_vec_at(&global.config.key_bindings, x);
		switch (current_mapping->action->value) {


			// ============================================================================== BOOL ==============================================================================
			case IV_bool: {

				// handle input
				loc_key_code = c_vec_at(current_mapping->key_codes, 0);

				__update_key_state(current_mapping, keybord_state[*loc_key_code]);

				// ------------------------------------------------- handle triggers -------------------------------------------------
				if(current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_KEY_DOWN)
					current_mapping->action->boolean = (bool)keybord_state[*loc_key_code];

				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_KEY_UP)
					current_mapping->action->boolean = !(bool)keybord_state[*loc_key_code];

				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_HOLD) {

					current_mapping->action->boolean = 
						(bool)((Calc_Time_Dif_in_Seconds(current_mapping->time_stamp, Get_TimeStamp()) >= current_mapping->settings.duration_in_sec) && current_mapping->key_state == KS_held);
				}

				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_TAP) {

					if (Calc_Time_Dif_in_Seconds(current_mapping->time_stamp, Get_TimeStamp()) <= current_mapping->settings.duration_in_sec
						&& current_mapping->key_state == KS_released) {

						current_mapping->action->boolean = true;
						SET_TIMESTAMP_TO_PAST;
					}

					else
						current_mapping->action->boolean = false;
				}

				// ------------------------------------------------- handle modefiers -------------------------------------------------

				if (current_mapping->settings.modefiers_flags & INPUT_ACTION_MODEFIER_NEGATE)
					current_mapping->action->boolean = !current_mapping->action->boolean;

			} break;

			// ============================================================================== FLOAT ==============================================================================
			case IV_float: {

				__update_key_state(current_mapping,
					keybord_state[*(u8*)c_vec_at(current_mapping->key_codes, 0)] ||
					keybord_state[*(u8*)c_vec_at(current_mapping->key_codes, 1)]);

				float buffer = current_mapping->action->floating_point;
				float target = 0.0f;
				current_mapping->action->floating_point = 0.0f;

				// ------------------------------------------------- handle triggers -------------------------------------------------
				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_KEY_DOWN)
					UPDATE_INPUT_FLOAT(true && )

				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_KEY_UP)
					UPDATE_INPUT_FLOAT(!)

				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_HOLD) {

					if ((Calc_Time_Dif_in_Seconds(current_mapping->time_stamp, Get_TimeStamp()) >= current_mapping->settings.duration_in_sec) && current_mapping->key_state == KS_held)
						UPDATE_INPUT_FLOAT(true &&)
				}

				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_TAP) {

					if (Calc_Time_Dif_in_Seconds(current_mapping->time_stamp, Get_TimeStamp()) <= current_mapping->settings.duration_in_sec
						&& current_mapping->key_state == KS_released) {

						if (current_mapping->buffer & BIT(0))
							target += 1.0f;
						if (current_mapping->buffer & BIT(1))
							target -= 1.0f;
						SET_TIMESTAMP_TO_PAST;
					}

					else if (current_mapping->key_state > 0) {

						current_mapping->buffer = 0;
						BIT_ENCODE_KEYBORD_STATE(0);
						BIT_ENCODE_KEYBORD_STATE(1);
					}
				}

				// ------------------------------------------------- handle modefiers -------------------------------------------------
				if (current_mapping->settings.modefiers_flags & INPUT_ACTION_MODEFIER_NEGATE)
					target *= -1.0f;

				if (current_mapping->settings.modefiers_flags & INPUT_ACTION_MODEFIER_SMOOTH) {

					if (fabs(target - buffer) > MIN_DIF_BETWEEN_SMOOTH_INPUT_AND_TARGET) {

						if (target > buffer)
							CHANGE_VAR_BY_DELTA(, +);
						else
							CHANGE_VAR_BY_DELTA(, -);
					}
				}

				buffer = target;
				current_mapping->action->floating_point = clamp_f_get(buffer, -1, 1);

			} break;


			// ========================================================================================= VEC 2 =========================================================================================
			case IV_vec2: {

				__update_key_state(current_mapping,
					keybord_state[*(u8*)c_vec_at(current_mapping->key_codes, 0)] ||
					keybord_state[*(u8*)c_vec_at(current_mapping->key_codes, 1)] ||
					keybord_state[*(u8*)c_vec_at(current_mapping->key_codes, 2)] ||
					keybord_state[*(u8*)c_vec_at(current_mapping->key_codes, 3)]);

				vec2 buffer = { 0 };
				buffer[0] = current_mapping->action->vector2D[0];
				buffer[1] = current_mapping->action->vector2D[1];
				vec2 target = { 0 };
				current_mapping->action->vector2D[0] = 0.0f;
				current_mapping->action->vector2D[1] = 0.0f;

				// ------------------------------------------------- handle triggers -------------------------------------------------
				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_KEY_DOWN)
					UPDATE_INPUT_VECTOR(true &&)

				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_KEY_UP)
					UPDATE_INPUT_VECTOR(!)

				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_HOLD) {

					if ((Calc_Time_Dif_in_Seconds(current_mapping->time_stamp, Get_TimeStamp()) >= current_mapping->settings.duration_in_sec) && current_mapping->key_state == KS_held)
						UPDATE_INPUT_VECTOR(true &&)
				}

				if (current_mapping->settings.trigger_flags & INPUT_ACTION_TRIGGER_TAP) {

					if (Calc_Time_Dif_in_Seconds(current_mapping->time_stamp, Get_TimeStamp()) <= current_mapping->settings.duration_in_sec
						&& current_mapping->key_state == KS_released) {

						if (current_mapping->buffer & BIT(0))
							target[0] += 1.0f;
						if (current_mapping->buffer & BIT(1))
							target[0] -= 1.0f;
						if (current_mapping->buffer & BIT(2))
							target[1] += 1.0f;
						if (current_mapping->buffer & BIT(3))
							target[1] -= 1.0f;
						SET_TIMESTAMP_TO_PAST;
					}

					else if (current_mapping->key_state > 0) {

						current_mapping->buffer = 0;
						BIT_ENCODE_KEYBORD_STATE(0);
						BIT_ENCODE_KEYBORD_STATE(1);
						BIT_ENCODE_KEYBORD_STATE(2);
						BIT_ENCODE_KEYBORD_STATE(3);
						
					}
				}

				// ------------------------------------------------- handle modefiers -------------------------------------------------
				if (current_mapping->settings.modefiers_flags & INPUT_ACTION_MODEFIER_NEGATE)
					vec2_scale(target, target, -1.0f);


				if (current_mapping->settings.modefiers_flags & INPUT_ACTION_MODEFIER_VEC2_NORMAL) {

					float vec_length = vec2_len(target);
					if (vec_length > 0.0f) 
						vec2_scale(target, target, 1.f / vec_length);
				}

				if (current_mapping->settings.modefiers_flags & INPUT_ACTION_MODEFIER_SMOOTH) {

					if (fabs(target[0] - buffer[0]) > MIN_DIF_BETWEEN_SMOOTH_INPUT_AND_TARGET ||
						fabs(target[1] - buffer[1]) > MIN_DIF_BETWEEN_SMOOTH_INPUT_AND_TARGET) {


						if (target[0] > buffer[0])
							CHANGE_VAR_BY_DELTA([0], +);
						else
							CHANGE_VAR_BY_DELTA([0], -);

						if (target[1] > buffer[1])
							CHANGE_VAR_BY_DELTA([1], +);
						else
							CHANGE_VAR_BY_DELTA([1], -);
					}
				}

				buffer[0] = target[0];
				buffer[1] = target[1];
				current_mapping->action->vector2D[0] = clamp_f_get(buffer[0], -1, 1);
				current_mapping->action->vector2D[1] = clamp_f_get(buffer[1], -1, 1);

			} break;

			default:
				break;
		}
	}
}

// ------------------------------------------------------------------------------------------ privete funcs ------------------------------------------------------------------------------------------

//
static inline void __update_key_state(input_mapping* mapping, const bool is_pressed) {

	if (is_pressed) {
		if (mapping->key_state > 0)
			mapping->key_state = KS_held;
		else {

			mapping->key_state = KS_pressed;
			mapping->time_stamp = Get_TimeStamp();
		}
	} else
		mapping->key_state = KS_released;
}