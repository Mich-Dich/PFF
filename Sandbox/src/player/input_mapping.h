#pragma once

#include "engine/io_handler/input_action.h"

class default_IAM: public PFF::input_mapping {

public:
	default_IAM();
	~default_IAM();

	PFF::input_action move;
	PFF::input_action look;

};

default_IAM::default_IAM() {

	move = PFF::input_action{};
	move.triger_when_paused = false;
	move.value = PFF::input_value::IV_vec2;
	move.duration_in_sec = 0.5f;
	move.keys = {
		{key_code::key_W, INPUT_ACTION_TRIGGER_KEY_DOWN },
		{key_code::key_S, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_NEGATE },
		{key_code::key_A, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS},
		{key_code::key_D, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS | INPUT_ACTION_MODEFIER_NEGATE},
	};
	REGISTER_INPUT_ACTION(move);


	look = PFF::input_action{};
	look.triger_when_paused = false;
	look.value = PFF::input_value::IV_bool;
	look.duration_in_sec = 0.5f;
	look.keys = {
		{key_code::key_I, INPUT_ACTION_TRIGGER_KEY_DOWN},
	};
	REGISTER_INPUT_ACTION(look);

}

default_IAM::~default_IAM() {
}
