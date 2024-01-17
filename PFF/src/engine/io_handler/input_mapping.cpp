
#include "util/pffpch.h"

#include "input_action.h"
#include "input_mapping.h"

namespace PFF {

	input_mapping::input_mapping() {

		m_actions.clear();
	}
	
	void input_mapping::register_action(input_action* action) {

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

		LOAD_CONFIG_BOOL(input, action->triger_when_paused, action->name, "triger_when_paused");
		LOAD_CONFIG_NUM(input, action->duration_in_sec, f32, action->name, "duration_in_sec");

		u32 buffer_input_val = static_cast<u32>(action->value);
		LOAD_CONFIG_NUM(input, buffer_input_val, u32, action->name, "input_value");
		action->value = static_cast<input_value>(buffer_input_val);

		size_t buffer_array_size = action->keys.size();
		LOAD_CONFIG_NUM(input, buffer_array_size, size_t, action->name, "key_details_size");
		action->keys.resize(buffer_array_size);

		for (u32 x = 0; x < buffer_array_size; x++) {

			u32 buffer_key_code = static_cast<u32>(action->keys[x].key);
			LOAD_CONFIG_NUM(input, buffer_key_code, u32, action->name, "key_" + std::to_string(x) + "_key_code");
			action->keys[x].key = static_cast<key_code>(buffer_key_code);

			//LOAD_CONFIG_NUM(input, action->keys[x].duration_in_sec, f32, name, "key_" + std::to_string(x) + "_duration_in_sec");
			LOAD_CONFIG_NUM(input, action->keys[x].trigger_flags, u16, action->name, "key_" + std::to_string(x) + "_trigger_flags");
			LOAD_CONFIG_NUM(input, action->keys[x].modefier_flags, u16, action->name, "key_" + std::to_string(x) + "_modefier_flags");
		}

		// m_actions.push_back(action);
		m_actions.emplace_back(action);
	}

}
