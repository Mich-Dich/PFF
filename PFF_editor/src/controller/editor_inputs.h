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
		PFF::input_action toggle_fps;

	};

	editor_inputs::editor_inputs() {

		move = PFF::input_action{};
		move.description = "reposition the editor camera";
		move.triger_when_paused = false;
		move.flags = INPUT_ACTION_MODEFIER_SMOOTH_INTERP | INPUT_ACTION_MODEFIER_USE_VEC_NORMAL | INPUT_ACTION_MODEFIER_AUTO_RESET;
		move.value = PFF::input::action_type::vec_3D;
		move.duration_in_sec = 0.5f;
		move.keys_bindings = {
			{key_code::key_W, INPUT_ACTION_TRIGGER_KEY_DOWN, INPUT_ACTION_MODEFIER_NEGATE},
			{key_code::key_S, INPUT_ACTION_TRIGGER_KEY_DOWN},
			{key_code::key_A, INPUT_ACTION_TRIGGER_KEY_DOWN, INPUT_ACTION_MODEFIER_AXIS_2 | INPUT_ACTION_MODEFIER_NEGATE},
			{key_code::key_D, INPUT_ACTION_TRIGGER_KEY_DOWN, INPUT_ACTION_MODEFIER_AXIS_2},
			{key_code::key_space, INPUT_ACTION_TRIGGER_KEY_DOWN, INPUT_ACTION_MODEFIER_AXIS_3 | INPUT_ACTION_MODEFIER_NEGATE},
			{key_code::key_left_shift, INPUT_ACTION_TRIGGER_KEY_DOWN, INPUT_ACTION_MODEFIER_AXIS_3 },
		};								
		REGISTER_INPUT_ACTION(move);


		capture_mouse = PFF::input_action{};
		capture_mouse.description = "left clicl to change the camera orientation";
		capture_mouse.triger_when_paused = false;
		capture_mouse.flags = 0;
		capture_mouse.value = PFF::input::action_type::boolean;
		capture_mouse.duration_in_sec = 0.5f;
		capture_mouse.keys_bindings = {
			{key_code::mouse_bu_right, INPUT_ACTION_TRIGGER_KEY_DOWN},
		};
		REGISTER_INPUT_ACTION(capture_mouse);


		look = PFF::input_action{};
		look.description = "change the direction of the editor camera";
		look.triger_when_paused = false;
		look.flags = INPUT_ACTION_MODEFIER_AUTO_RESET_ALL | INPUT_ACTION_MODEFIER_SMOOTH_INTERP;
		look.value = PFF::input::action_type::vec_2D;
		look.duration_in_sec = 0.5f;
		look.keys_bindings = {
			{key_code::mouse_moved_x, INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG, INPUT_ACTION_MODEFIER_NEGATE},
			{key_code::mouse_moved_y, INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG, INPUT_ACTION_MODEFIER_AXIS_2 | INPUT_ACTION_MODEFIER_NEGATE},
		};
		REGISTER_INPUT_ACTION(look);

		change_move_speed = PFF::input_action{};
		change_move_speed.description = "increase/decrease the movement speed of the editor camera";
		change_move_speed.triger_when_paused = false;
		change_move_speed.flags = INPUT_ACTION_MODEFIER_AUTO_RESET_ALL;
		change_move_speed.value = PFF::input::action_type::vec_1D;
		change_move_speed.duration_in_sec = 0.5f;
		change_move_speed.keys_bindings = {
			{key_code::mouse_scrolled_y, INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG},
		};
		REGISTER_INPUT_ACTION(change_move_speed);



		toggle_fps = PFF::input_action{};
		toggle_fps.description = "toggle the fps limiter";
		toggle_fps.triger_when_paused = false;
		toggle_fps.flags = INPUT_ACTION_MODEFIER_AUTO_RESET_ALL;
		toggle_fps.value = PFF::input::action_type::boolean;
		toggle_fps.duration_in_sec = 0.f;
		toggle_fps.keys_bindings = {
			{key_code::key_P, INPUT_ACTION_TRIGGER_KEY_MOVE_DOWN},
		};
		REGISTER_INPUT_ACTION(toggle_fps);

	}

	editor_inputs::~editor_inputs() {}

}
