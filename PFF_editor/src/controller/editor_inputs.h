#pragma once

#include "engine/io_handler/input_mapping.h"

namespace PFF {

	class editor_inputs: public PFF::input_mapping {
	public:

		editor_inputs();
		~editor_inputs();

		PFF::input_action move;
		PFF::input_action capture_mouse;
		PFF::input_action change_move_speed;
		PFF::input_action look;

	};

	editor_inputs::editor_inputs() {

		move = PFF::input_action{};
		move.description = "reposition the editor camera";
		move.triger_when_paused = false;
		move.modefier_flags = INPUT_ACTION_MODEFIER_SMOOTH_INTERP | INPUT_ACTION_MODEFIER_VEC_NORMAL | INPUT_ACTION_MODEFIER_AUTO_RESET;
		move.value = PFF::input_value::_3D;
		move.duration_in_sec = 0.5f;
		move.keys = {
			{key_code::key_W, INPUT_ACTION_TRIGGER_KEY_DOWN },
			{key_code::key_S, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_NEGATE },
			{key_code::key_A, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_AXIS_2 | INPUT_ACTION_MODEFIER_NEGATE},
			{key_code::key_D, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_AXIS_2},
			{key_code::key_space, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_AXIS_3},
			{key_code::key_left_shift, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_AXIS_3 | INPUT_ACTION_MODEFIER_NEGATE},
		};								
		REGISTER_INPUT_ACTION(move);


		capture_mouse = PFF::input_action{};
		capture_mouse.description = "left clicl to change the camera direction";
		capture_mouse.triger_when_paused = false;
		capture_mouse.modefier_flags = 0;
		capture_mouse.value = PFF::input_value::_bool;
		capture_mouse.duration_in_sec = 0.5f;
		capture_mouse.keys = {
			{key_code::mouse_bu_right, INPUT_ACTION_TRIGGER_KEY_DOWN},
		};
		REGISTER_INPUT_ACTION(capture_mouse);


		change_move_speed = PFF::input_action{};
		change_move_speed.description = "increase/decrease the movement speed of the editor camera";
		change_move_speed.triger_when_paused = false;
		change_move_speed.modefier_flags = INPUT_ACTION_MODEFIER_AUTO_RESET_ALL;
		change_move_speed.value = PFF::input_value::_1D;
		change_move_speed.duration_in_sec = 0.5f;
		change_move_speed.keys = {
			{key_code::mouse_scrolled_y, INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG},
		};
		REGISTER_INPUT_ACTION(change_move_speed);


		look = PFF::input_action{};
		look.description = "change the direction of the editor camera";
		look.triger_when_paused = false;
		look.modefier_flags = INPUT_ACTION_MODEFIER_AUTO_RESET_ALL | INPUT_ACTION_MODEFIER_SMOOTH_INTERP;
		look.value = PFF::input_value::_2D;
		look.duration_in_sec = 0.5f;
		look.keys = {
			{key_code::mouse_moved_x, INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG},
			{key_code::mouse_moved_y, INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG , INPUT_ACTION_MODEFIER_AXIS_2 | INPUT_ACTION_MODEFIER_NEGATE},
		};
		REGISTER_INPUT_ACTION(look);

	}

	editor_inputs::~editor_inputs() {

		// UNREGISTER_INPUT_ACTION(move);
		// UNREGISTER_INPUT_ACTION(capture_mouse);
		// UNREGISTER_INPUT_ACTION(look);
	}

}
