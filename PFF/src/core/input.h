#pragma once

typedef enum {
	IK_left,
	IK_right,
	IK_up,
	IK_down,
	IK_jump,
	IK_escape
} Input_key;

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
} Input_State;

void update_input(void);