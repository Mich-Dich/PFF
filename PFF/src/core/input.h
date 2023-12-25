#pragma once

#include "core/pch.h"
#include "core/types.h"
#include "core/container/darray.h"

// --------------------------------------------------- General Action Structs ---------------------------------------------------

typedef enum {
	IAT_key_down = 0,
	IAT_key_up,
	IAT_hold,		// hold for min duration
	IAT_tap			// down & up in short time
} input_action_trigger;

#define BIT(x) (1 << x)

#define INPUT_ACTION_TRIGGER_KEY_DOWN		BIT(0)
#define INPUT_ACTION_TRIGGER_KEY_UP			BIT(1)
#define INPUT_ACTION_TRIGGER_HOLD			BIT(2)
#define INPUT_ACTION_TRIGGER_TAP			BIT(3)

#define INPUT_ACTION_MODEFIER_NEGATE		BIT(0)
#define INPUT_ACTION_MODEFIER_VEC2_NORMAL	BIT(1)
#define INPUT_ACTION_MODEFIER_SMOOTH		BIT(2)


#define MIN_DIF_BETWEEN_SMOOTH_INPUT_AND_TARGET		0.08

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

// --------------------------------------------------- Input Action ---------------------------------------------------

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
void register_key_binding_bool(const char* key, input_action* action);
void register_key_binding_float(const char* plus, const char* minus, input_action* action);
void register_key_binding_vec2(const char* x_plus, const char* x_minus, const char* y_plus, const char* y_minus, input_action* action);
void change_key_binding_Settings(input_mapping mapping, const input_action_settings settings);

// --------------------------------------------------- OLD SHITY SYSTEM ---------------------------------------------------

typedef enum {
	IK_left,
	IK_right,
	IK_up,
	IK_down,
	IK_jump,
	IK_escape
} input_key;


typedef struct {
	Key_state left;
	Key_state right;
	Key_state up;
	Key_state down;
	Key_state escape;
} input_State;

void input_update(void);