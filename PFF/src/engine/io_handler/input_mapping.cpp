
#include "util/pffpch.h"

#include "input_action.h"
#include "input_mapping.h"

namespace PFF {

	//bool input_mapping::input_register_action_bool(key_code key, input_action* action) {
	//
	//	char config_field_name[100] = { 0 };
	//	char config_value[100] = { 0 };
	//
	//	//SAVE_CONFIGURATION("%s", false, key);
	//	//TRY_LOAD_CONFIGURATION(config_value, key);
	//
	//	CORE_VALIDATE(key == key_code::key_unknown, return false, "scan_code: ", "Invalid scan code when binding key: ");
	//	/*
	//	action->value = IV_bool;
	//	input_mapping new_mapping = { 0 };
	//	new_mapping.key_codes = c_vec_new(sizeof(u8), 1);
	//	c_vec_push_back(new_mapping.key_codes, &scan_code);
	//
	//	new_mapping.override_settings = false;
	//	new_mapping.settings = action->settings;
	//	new_mapping.action = action;
	//	c_vec_push_back(&global.input.input_actions, &new_mapping);*/
	//	return true;
	//}

	/*
		bool triger_when_paused;
		input_value value;
		union {
			bool boolean;
			f32 axis_1d;
			glm::vec2 axis_2d{};
		} data;
		std::chrono::time_point<std::chrono::high_resolution_clock> time_stamp;
		f32 duration_in_sec;
		std::vector<key_details> keys;
	*/

	void input_mapping::register_action(std::string name, input_action& action) {

		//  bool triger_when_paused;													// SAVE
		//  input_value value;															// SAVE
		//  union {																		// DOESNT NEED TO BE SAVED
		//  	bool boolean;															// DOESNT NEED TO BE SAVED
		//  	f32 axis_1d;															// DOESNT NEED TO BE SAVED
		//  	glm::vec2 axis_2d{};													// DOESNT NEED TO BE SAVED
		//  } data;																		// DOESNT NEED TO BE SAVED
		//  std::chrono::time_point<std::chrono::high_resolution_clock> time_stamp;		// DOESNT NEED TO BE SAVED
		//  f32 duration_in_sec;														// SAVE
		//  std::vector<key_details> keys;												// SAVE

		LOAD_CONFIG_BOOL(input, action.triger_when_paused, name, "triger_when_paused");
		LOAD_CONFIG_NUM(input, action.duration_in_sec, f32, name, "duration_in_sec");

		u32 buffer_input_val = static_cast<u32>(action.value);
		LOAD_CONFIG_NUM(input, buffer_input_val, u32, name, "input_value");
		action.value = static_cast<input_value>(buffer_input_val);

		size_t buffer_array_size = action.keys.size();
		LOAD_CONFIG_NUM(input, buffer_array_size, size_t, name, "key_details_size");
		action.keys.resize(buffer_array_size);

		for (u32 x = 0; x < buffer_array_size; x++) {

			u32 buffer_key_code = static_cast<u32>(action.keys[x].key);
			LOAD_CONFIG_NUM(input, buffer_key_code, u32, name, "key_" + std::to_string(x) + "_key_code");
			action.keys[x].key = static_cast<key_code>(buffer_key_code);

			//LOAD_CONFIG_NUM(input, action.keys[x].duration_in_sec, f32, name, "key_" + std::to_string(x) + "_duration_in_sec");
			LOAD_CONFIG_NUM(input, action.keys[x].trigger_flags, u16, name, "key_" + std::to_string(x) + "_trigger_flags");
			LOAD_CONFIG_NUM(input, action.keys[x].modefier_flags, u16, name, "key_" + std::to_string(x) + "_modefier_flags");
		}

		m_actions.push_back(action);
	}

}


