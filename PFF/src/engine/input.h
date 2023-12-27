#pragma once

#include "pch.h"

// --------------------------------------------------- General Action Structs ---------------------------------------------------

#define INPUT_ACTION_TRIGGER_KEY_DOWN		BIT(0)
#define INPUT_ACTION_TRIGGER_KEY_UP			BIT(1)
#define INPUT_ACTION_TRIGGER_HOLD			BIT(2)
#define INPUT_ACTION_TRIGGER_TAP			BIT(3)

#define INPUT_ACTION_MODEFIER_NEGATE		BIT(0)
#define INPUT_ACTION_MODEFIER_VEC2_NORMAL	BIT(1)
#define INPUT_ACTION_MODEFIER_SMOOTH		BIT(2)


#define MIN_DIF_BETWEEN_SMOOTH_INPUT_AND_TARGET		0.08
#define INPUT_ACTION_REGISTER_NAME(var)         var.action_name = #var


typedef struct {
	c_vector input_actions;
	i32 courser_pos_x;
	i32 courser_pos_y;
	bool courser_in_window;
} input_State;


// --------------------------------------------------- Input Action ---------------------------------------------------

typedef struct {
	float duration_in_sec;
	u32 trigger_flags;
	u32 modefiers_flags;
} input_action_settings;

typedef enum {
	KS_released,
	KS_pressed,
	KS_held
} Key_state;

typedef enum {
	IV_bool = 0,
	IV_float,
	IV_vec2
} input_value;

typedef	struct {
	char* action_name;
	char* description;
	bool triger_when_paused;
	input_value value;
	union value_type {
		bool boolean;
		float floating_point;
		vec2 vector2D;
	};
	input_action_settings settings;
} input_action;

// --------------------------------------------------- Input Mapping ---------------------------------------------------

typedef	struct {
	c_vector* key_codes;
	Key_state key_state;
	input_action* action;
	bool override_settings;
	input_action_settings settings;
	u64 time_stamp;
	u16 buffer;
} input_mapping;


void input_init(void);
void input_update(void);

PFF_API void input_register_action_bool(const char* key, input_action* action);
PFF_API void input_register_action_float(const char* plus, const char* minus, input_action* action);
PFF_API void input_register_action_vec2(const char* x_plus, const char* x_minus, const char* y_plus, const char* y_minus, input_action* action);

PFF_API void input_change_action_settings(input_mapping mapping, const input_action_settings settings);
PFF_API bool input_update_action_binding_bool(input_action* action, char* key);
PFF_API bool input_update_action_binding_float(input_action* action, const char* plus, const char* minus);
PFF_API bool input_update_action_binding_vec2(input_action* action, const char* x_plus, const char* x_minus, const char* y_plus, const char* y_minus);

