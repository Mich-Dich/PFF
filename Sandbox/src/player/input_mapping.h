#pragma once


class default_IAM: public PFF::input_mapping {

public:
	default_IAM();
	~default_IAM();

	PFF::input_action move;
	PFF::input_action look;

};

default_IAM::default_IAM() {

	move = PFF::input_action{};
	move.description = "this action handles the generic movement (forward/back/rght/left)";	// optinal
	move.triger_when_paused = false;														// optinal
	move.keys = {
		{key_code::key_W, INPUT_ACTION_TRIGGER_KEY_DOWN },
		{key_code::key_S, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_NEGATE},
		{key_code::key_A, INPUT_ACTION_TRIGGER_KEY_DOWN },
		{key_code::key_D, INPUT_ACTION_TRIGGER_KEY_DOWN , INPUT_ACTION_MODEFIER_NEGATE},
	};
	m_actions.push_back(move);
	LOG(Debug, "register input_action: move");

	look = PFF::input_action{};
	move.description = "this action handles the camera movement (mouse movement X/Y)";		// optinal
	move.triger_when_paused = false;														// optinal
	move.keys = {
		{key_code::key_W },
		{key_code::key_S },
	};
	m_actions.push_back(move);
	LOG(Debug, "register input_action: look");

}

default_IAM::~default_IAM() {
}
