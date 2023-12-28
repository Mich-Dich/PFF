
#include <SDL.h>

// #include "engine/config.h"
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

#define SAVE_CONFIGURATION(format, override, input_key)		sprintf_s(config_field_name, sizeof(config_field_name), format, action->action_name);			\
															CORE_VALIDATE(config_save_a_configuration(config_field_name, input_key, override) == EC_success, return false, "", "Failed to save to config")

#define TRY_LOAD_CONFIGURATION(config_value, key)				if (config_try_to_find(config_field_name, config_value) != EC_success) {					\
																strncpy_s(config_value, sizeof(config_value), key, 100);								\
															}


// ------------------------------------------------------------------------------------------ privete funcs ------------------------------------------------------------------------------------------

static inline void __update_key_state(input_mapping* mapping, const bool is_pressed);


// ========================================================================================== main functions ==========================================================================================

//
void input_init(void) {

	global.input.input_actions = *c_vec_new(sizeof(input_mapping), 1);

	CORE_LOG_INIT_SUBSYS("input")
}

//
void input_change_action_settings(input_mapping mapping, input_action_settings settings) {

	mapping.override_settings = true;
	mapping.settings = settings;
}

// ------------------------------------------------------------------------------------------ register bindings ------------------------------------------------------------------------------------------

// inital setup of key binding of a bool value
// This is a soft process, meaning it only registers this binding if the config file doesnt habe it already
bool input_register_action_bool(const char* key, input_action* action) {

	char config_field_name[100] = { 0 };
	char config_value[100] = { 0 };

	SAVE_CONFIGURATION("%s", false, key);
	TRY_LOAD_CONFIGURATION(config_value, key);

	SDL_Scancode scan_code = SDL_GetScancodeFromName(config_value);
	CORE_VALIDATE(scan_code != SDL_SCANCODE_UNKNOWN, return false, "scan_code: %s/ %d", "Invalid scan code when binding key: %s", key, scan_code);
	action->value = IV_bool;
	input_mapping new_mapping = { 0 };
	new_mapping.key_codes = c_vec_new(sizeof(u8), 1);
	c_vec_push_back(new_mapping.key_codes, &scan_code);

	new_mapping.override_settings = false;
	new_mapping.settings = action->settings;
	new_mapping.action = action;
	c_vec_push_back(&global.input.input_actions, &new_mapping);
	return true;
}

// inital setup of key binding of a float
// This is a soft process, meaning it only registers this binding if the config file doesnt habe it already
bool input_register_action_float(const char* plus, const char* minus, input_action* action) {

	CORE_VALIDATE(plus != minus, return false, "", "registering the same key multiple time is not supported");

	char config_field_name[100] = { 0 };
	char config_value_p[100] = { 0 };
	char config_value_m[100] = { 0 };

	SAVE_CONFIGURATION("%s_plus", false, plus);
	TRY_LOAD_CONFIGURATION(config_value_p, plus);
	SAVE_CONFIGURATION("%s_minus", false, minus);
	TRY_LOAD_CONFIGURATION(config_value_m, minus);

	SDL_Scancode scan_code_p = SDL_GetScancodeFromName(config_value_p);
	SDL_Scancode scan_code_m = SDL_GetScancodeFromName(config_value_m);
	CORE_VALIDATE(scan_code_p != SDL_SCANCODE_UNKNOWN && scan_code_m != SDL_SCANCODE_UNKNOWN,
		return false, "keys [%s %s] are valid", "Invalid scan code when binding keys: [%s %s]", plus, minus);

	action->value = IV_float;
	input_mapping new_mapping = { 0 };
	new_mapping.key_codes = c_vec_new(sizeof(u8), 2);
	c_vec_push_back(new_mapping.key_codes, &scan_code_p);
	c_vec_push_back(new_mapping.key_codes, &scan_code_m);

	new_mapping.override_settings = false;
	new_mapping.settings = action->settings;
	new_mapping.action = action;
	c_vec_push_back(&global.input.input_actions, &new_mapping);
	return true;
}

// inital setup of key binding of a vec2
// This is a soft process, meaning it only registers this binding if the config file doesnt habe it already
bool input_register_action_vec2(const char* x_plus, const char* x_minus, const char* y_plus, const char* y_minus, input_action* action) {

	CORE_VALIDATE((x_plus != x_minus) && (y_plus != y_minus) && (x_plus != y_plus) && (x_minus != y_minus), return false, "", "registering the same key multiple time is not supported");

	char config_field_name[100] = { 0 };
	char config_value_xp[100] = { 0 };
	char config_value_xm[100] = { 0 };
	char config_value_yp[100] = { 0 };
	char config_value_ym[100] = { 0 };

	SAVE_CONFIGURATION("%s_x_plus", false, x_plus);
	TRY_LOAD_CONFIGURATION(config_value_xp, x_plus);
	SAVE_CONFIGURATION("%s_x_minus", false, x_minus);
	TRY_LOAD_CONFIGURATION(config_value_xm, x_minus);
	SAVE_CONFIGURATION("%s_y_plus", false, y_plus);
	TRY_LOAD_CONFIGURATION(config_value_yp, y_plus);
	SAVE_CONFIGURATION("%s_y_minus", false, y_minus);
	TRY_LOAD_CONFIGURATION(config_value_ym, y_minus);

	//sprintf_s(&config_field_name, sizeof(config_field_name), "%s_x_plus", action->action_name);
	//CORE_VALIDATE(config_save_a_configuration(config_field_name, x_plus, false) == EC_success, return, "", "Failed to save to config");

	SDL_Scancode scan_code_xp = SDL_GetScancodeFromName(config_value_xp);
	SDL_Scancode scan_code_xm = SDL_GetScancodeFromName(config_value_xm);
	SDL_Scancode scan_code_yp = SDL_GetScancodeFromName(config_value_yp);
	SDL_Scancode scan_code_ym = SDL_GetScancodeFromName(config_value_ym);
	CORE_VALIDATE(scan_code_xp != SDL_SCANCODE_UNKNOWN && scan_code_xm != SDL_SCANCODE_UNKNOWN && scan_code_yp != SDL_SCANCODE_UNKNOWN && scan_code_ym != SDL_SCANCODE_UNKNOWN,
		return false, "keys [%s %s %s %s] are valid", "Invalid scan code when binding keys: [%s %s %s %s]", x_plus, x_minus, y_plus, y_minus);

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
	c_vec_push_back(&global.input.input_actions, &new_mapping);
	return true;
}

// ------------------------------------------------------------------------------------------ override bindings ------------------------------------------------------------------------------------------

//
bool input_update_action_binding_bool(input_action* action, char* key) {

	SDL_Scancode scan_code = SDL_GetScancodeFromName(key);
	CORE_VALIDATE(scan_code != SDL_SCANCODE_UNKNOWN, return false, "scan_code: %s/ %d", "Failed to update to value saved in config", key, scan_code);

	for (u32 x = 0; x < global.input.input_actions.size; x++) {

		input_mapping* loc_mapping = c_vec_at(&global.input.input_actions, x);
		if (strncmp(loc_mapping->action->action_name, action->action_name, 50) == 0) {

			c_vec_clear(loc_mapping->key_codes);
			c_vec_push_back(loc_mapping->key_codes, &scan_code);
			CORE_LOG(Info, "Updating binding for [%s] to [%s/%d] (key/code)", action->action_name, key, scan_code);
			break;
		}
	}
	char config_field_name[100] = { 0 };
	SAVE_CONFIGURATION("%s", true, key);
	return true;
}

//
bool input_update_action_binding_float(input_action* action, const char* plus, const char* minus) {

	CORE_VALIDATE(plus != minus, return false, "", "registering the same key multiple time is not supported");

	SDL_Scancode scan_code_p = SDL_GetScancodeFromName(plus);
	SDL_Scancode scan_code_m = SDL_GetScancodeFromName(minus);
	CORE_VALIDATE(scan_code_p != SDL_SCANCODE_UNKNOWN && scan_code_m != SDL_SCANCODE_UNKNOWN,
		return false, "keys [%s %s] are valid", "Invalid scan code when binding keys: [%s %s]", plus, minus);

	for (u32 x = 0; x < global.input.input_actions.size; x++) {

		input_mapping* loc_mapping = c_vec_at(&global.input.input_actions, x);
		if (strncmp(loc_mapping->action->action_name, action->action_name, 50) == 0) {

			c_vec_clear(loc_mapping->key_codes);
			c_vec_push_back(loc_mapping->key_codes, &scan_code_p);
			c_vec_push_back(loc_mapping->key_codes, &scan_code_m);
			CORE_LOG(Info, "Updating binding for [%s] to [%s/%d  %s/%d] (key/code)", action->action_name, plus, scan_code_p, minus, scan_code_m);
			break;
		}
	}
	char config_field_name[100] = { 0 };
	SAVE_CONFIGURATION("%s_plus", true, plus);
	SAVE_CONFIGURATION("%s_minus", true, minus);
	return true;
}

//
bool input_update_action_binding_vec2(input_action* action, const char* x_plus, const char* x_minus, const char* y_plus, const char* y_minus) {

	CORE_VALIDATE((x_plus != x_minus) && (y_plus != y_minus) && (x_plus != y_plus) && (x_minus != y_minus), return false, "", "registering the same key multiple time is not supported");

	SDL_Scancode scan_code_xp = SDL_GetScancodeFromName(x_plus);
	SDL_Scancode scan_code_xm = SDL_GetScancodeFromName(x_minus);
	SDL_Scancode scan_code_yp = SDL_GetScancodeFromName(y_plus);
	SDL_Scancode scan_code_ym = SDL_GetScancodeFromName(y_minus);
	CORE_VALIDATE(scan_code_xp != SDL_SCANCODE_UNKNOWN && scan_code_xm != SDL_SCANCODE_UNKNOWN && scan_code_yp != SDL_SCANCODE_UNKNOWN && scan_code_ym != SDL_SCANCODE_UNKNOWN,
		return false, "keys [%s %s %s %s] are valid", "Invalid scan code when binding keys: [%s %s %s %s]", x_plus, x_minus, y_plus, y_minus);

	for (u32 x = 0; x < global.input.input_actions.size; x++) {

		input_mapping* loc_mapping = c_vec_at(&global.input.input_actions, x);
		if (strncmp(loc_mapping->action->action_name, action->action_name, 50) == 0) {

			c_vec_clear(loc_mapping->key_codes);
			c_vec_push_back(loc_mapping->key_codes, &scan_code_xp);
			c_vec_push_back(loc_mapping->key_codes, &scan_code_xm);
			c_vec_push_back(loc_mapping->key_codes, &scan_code_yp);
			c_vec_push_back(loc_mapping->key_codes, &scan_code_ym);
			CORE_LOG(Info, "Updating binding for [%s] to [%s/%d  %s/%d  %s/%d  %s/%d] (key/code)",
				action->action_name, x_plus, scan_code_xp, x_minus, scan_code_xm, y_plus, scan_code_yp, y_minus, scan_code_ym);
			break;
		}
	}
	char config_field_name[100] = { 0 };
	SAVE_CONFIGURATION("%s_x_plus", true, x_plus);
	SAVE_CONFIGURATION("%s_x_minus", true, x_minus);
	SAVE_CONFIGURATION("%s_y_plus", true, y_plus);
	SAVE_CONFIGURATION("%s_y_minus", true, y_minus);
	return true;
}

// ------------------------------------------------------------------------------------------ update input ------------------------------------------------------------------------------------------

//
void input_update(void) {


	SDL_GetMouseState(&global.input.courser_pos_x, &global.input.courser_pos_y);
	global.input.courser_in_window = (SDL_GetMouseFocus() != NULL);

	const u8* keybord_state = SDL_GetKeyboardState(NULL);

	input_mapping* current_mapping = { 0 };
	u8 loc_key_state = 0;
	u8* loc_key_code = 0;
	for (u16 x = 0; x < c_vec_size(&global.input.input_actions); ++x) {

		current_mapping = c_vec_at(&global.input.input_actions, x);
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