
#include "util/pffpch.h"

#include "input_action.h"
#include "input_mapping.h"

namespace PFF {

	bool input_mapping::input_register_action_bool(key_code key, input_action* action) {

		char config_field_name[100] = { 0 };
		char config_value[100] = { 0 };

		//SAVE_CONFIGURATION("%s", false, key);
		//TRY_LOAD_CONFIGURATION(config_value, key);

		CORE_VALIDATE(key == key_code::key_unknown, return false, "scan_code: ", "Invalid scan code when binding key: ");
		/*
		action->value = IV_bool;
		input_mapping new_mapping = { 0 };
		new_mapping.key_codes = c_vec_new(sizeof(u8), 1);
		c_vec_push_back(new_mapping.key_codes, &scan_code);

		new_mapping.override_settings = false;
		new_mapping.settings = action->settings;
		new_mapping.action = action;
		c_vec_push_back(&global.input.input_actions, &new_mapping);*/
		return true;
	}

}
