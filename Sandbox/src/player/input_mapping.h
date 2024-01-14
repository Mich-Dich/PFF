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

	//bool triger_when_paused;
	//input_value value;
	//union {
	//	bool boolean;
	//	f32 floating_point;
	//	glm::vec2 vector2D{};
	//} data;
	//std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
	//f32 duration_in_sec;
	//std::vector<key_details> keys;

	move = PFF::input_action{};
	move.triger_when_paused = false;
	move.value = PFF::input_value::IV_float;
	move.duration_in_sec = 0.5f;
	move.keys = {
		{key_code::key_W, INPUT_ACTION_TRIGGER_KEY_DOWN },
		{key_code::key_S, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_NEGATE },
		// {key_code::key_A, INPUT_ACTION_TRIGGER_KEY_DOWN },
		// {key_code::key_D, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_NEGATE},
	};
	register_action(move);


	look = PFF::input_action{};
	look.triger_when_paused = false;
	move.value = PFF::input_value::IV_bool;
	move.duration_in_sec = 0.5f;
	look.keys = {
		{key_code::key_T },
		{key_code::key_G },
	};
	register_action(look);

}

default_IAM::~default_IAM() {
}
