#pragma once


namespace PFF {

	class input_mapping {

	public:

		bool input_register_action_bool(key_code key, input_action* action);

	private:

		std::vector<input_action> inputs;

		std::vector<u16> key_codes;
		Key_state key_state;
		input_action* action;
		bool override_settings;
		input::settings settings;
		u64 time_stamp;
		u16 buffer;
	};

}