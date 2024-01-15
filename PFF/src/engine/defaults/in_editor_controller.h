#pragma once

#include "util/pffpch.h"
#include "engine/io_handler/input_mapping.h"
#include "engine/io_handler/input_action.h"


namespace PFF {

	class default_IAM : public input_mapping {

	public:
		default_IAM();
		~default_IAM();

		 input_action capture_mouse;
		 input_action look;
	};

	default_IAM::default_IAM() {

		capture_mouse =  input_action{};
		capture_mouse.triger_when_paused = false;
		capture_mouse.value =  input_value::IV_vec2;
		capture_mouse.duration_in_sec = 0.5f;
		capture_mouse.keys = {
			{key_code::mouse_bu_right, INPUT_ACTION_TRIGGER_KEY_DOWN},
		};
		REGISTER_INPUT_ACTION(capture_mouse);


		look =  input_action{};
		look.triger_when_paused = false;
		look.value =  input_value::IV_vec2;
		look.duration_in_sec = 0.5f;
		look.keys = {
			{key_code::mouse_moved},
		};
		REGISTER_INPUT_ACTION(look);

	}

	default_IAM::~default_IAM() {
	}

}