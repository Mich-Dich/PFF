#pragma once

#include "engine/io_handler/input_action.h"

namespace PFF {

	class default_IAM : public PFF::input_mapping {

	public:
		default_IAM();
		~default_IAM();

		PFF::input_action move;
		PFF::input_action capture_mouse;
		PFF::input_action look;

	};

	default_IAM::default_IAM() {

		move = PFF::input_action{};
		move.description = "Test description";
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


		capture_mouse = PFF::input_action{};
		move.description = "Test description";
		capture_mouse.triger_when_paused = false;
		capture_mouse.value = PFF::input_value::IV_bool;
		capture_mouse.duration_in_sec = 0.5f;
		capture_mouse.keys = {
			{key_code::mouse_bu_right, INPUT_ACTION_TRIGGER_KEY_DOWN},
		};
		REGISTER_INPUT_ACTION(capture_mouse);


		look = PFF::input_action{};
		move.description = "Test description";
		look.triger_when_paused = false;
		look.value = PFF::input_value::IV_vec2;
		look.duration_in_sec = 0.5f;
		look.keys = {
			{key_code::mouse_moved_x, INPUT_ACTION_TRIGGER_MOUSE_ANY_EVENT},
			{key_code::mouse_moved_y, INPUT_ACTION_TRIGGER_MOUSE_ANY_EVENT , INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS},
		};
		REGISTER_INPUT_ACTION(look);

	}

	default_IAM::~default_IAM() {
	}

}
