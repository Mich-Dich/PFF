#pragma once

#include "engine/io_handler/input_mapping.h"

namespace PFF {

	class editor_inputs: public PFF::input_mapping {
	public:

		editor_inputs();
		~editor_inputs();

		PFF::input_action move;
		PFF::input_action capture_mouse;
		PFF::input_action look;

	};

	editor_inputs::editor_inputs() {

		move = PFF::input_action{};
		move.description = "Test description";
		move.triger_when_paused = false;
		capture_mouse.modefier_flags = 0;
		move.value = PFF::input_value::_3D;
		move.duration_in_sec = 0.5f;
		move.keys = {
			{key_code::key_W, INPUT_ACTION_TRIGGER_KEY_DOWN },
			{key_code::key_S, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_NEGATE },
			{key_code::key_A, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS},
			{key_code::key_D, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS | INPUT_ACTION_MODEFIER_NEGATE},
			{key_code::key_space, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_VEC3_SECOND_AXIS},
			{key_code::key_left_shift, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_VEC3_SECOND_AXIS | INPUT_ACTION_MODEFIER_NEGATE},
		};
		REGISTER_INPUT_ACTION(move);


		capture_mouse = PFF::input_action{};
		capture_mouse.description = "Test description";
		capture_mouse.triger_when_paused = false;
		capture_mouse.modefier_flags = 0;
		capture_mouse.value = PFF::input_value::_bool;
		capture_mouse.duration_in_sec = 0.5f;
		capture_mouse.keys = {
			{key_code::mouse_bu_right, INPUT_ACTION_TRIGGER_KEY_DOWN},
		};
		REGISTER_INPUT_ACTION(capture_mouse);


		look = PFF::input_action{};
		look.description = "Test description";
		look.triger_when_paused = false;
		look.modefier_flags = INPUT_ACTION_MODEFIER_AUTO_RESET;
		look.value = PFF::input_value::_2D;
		look.duration_in_sec = 0.5f;
		look.keys = {
			{key_code::mouse_moved_x, INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG , INPUT_ACTION_MODEFIER_NEGATE},
			{key_code::mouse_moved_y, INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG , INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS},
		};
		REGISTER_INPUT_ACTION(look);

	}

	editor_inputs::~editor_inputs() {

		// UNREGISTER_INPUT_ACTION(move);
		// UNREGISTER_INPUT_ACTION(capture_mouse);
		// UNREGISTER_INPUT_ACTION(look);
	}

}
