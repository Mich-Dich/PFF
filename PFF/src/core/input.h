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

typedef enum {
	IAM_none = 0,
	IAM_negate,			// negate input value
	IAM_smooth			// smooth input over multiple frames
} input_action_modefier;

typedef struct {
	float duration;
	input_action_trigger trigger;
	input_action_modefier modefiers;
} input_action_settings;

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
		vec2 vector2;
	};
	input_action_settings settings;
} input_action;

// --------------------------------------------------- Input Mapping ---------------------------------------------------

typedef	struct {
	u8 key_code;
	input_action action;
	bool override_settings;
	input_action_settings settings;
} input_mapping;


void input_init(void);
void register_key_binding(const char* key, const input_action* action);
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

typedef enum {
	KS_released,
	KS_pressed,
	KS_held
} Key_state;

typedef struct {
	Key_state left;
	Key_state right;
	Key_state up;
	Key_state down;
	Key_state escape;
} input_State;

void update_input(void);