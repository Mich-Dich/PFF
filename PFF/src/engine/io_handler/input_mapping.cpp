
#include "util/pffpch.h"

#include "input_action.h"
#include "input_mapping.h"

namespace PFF {

	input_mapping::input_mapping() {

		PFF_PROFILE_FUNCTION();

		m_actions.clear();
	}
	
	void input_mapping::register_action(input_action* action, bool force_override) {

		PFF_PROFILE_FUNCTION();

		//  bool triger_when_paused;													// SAVE
		//  input_action_type value;													// SAVE
		//  union {																		// NO NEED TO SAVE
		//  	bool boolean;															// NO NEED TO SAVE
		//  	f32 axis_1d;															// NO NEED TO SAVE
		//  	glm::vec2 axis_2d{};													// NO NEED TO SAVE
		//  } data;																		// NO NEED TO SAVE
		//  std::chrono::time_point<std::chrono::high_resolution_clock> time_stamp;		// NO NEED TO SAVE
		//  f32 duration_in_sec;														// SAVE
		//  std::vector<key_details> keys;												// SAVE

		auto serializer_option = force_override ? serializer::option::save_to_file : serializer::option::load_from_file;

		serializer::yaml(config::get_filepath_from_configtype(config::file::input), action->name, serializer_option)
			.entry(KEY_VALUE(action->triger_when_paused))
			.entry(KEY_VALUE(action->duration_in_sec))
			.entry(KEY_VALUE(action->value))
			.vector(KEY_VALUE(action->keys), [&](serializer::yaml& yaml, u64 x) {
				yaml.entry(KEY_VALUE(action->keys[x].key));
				yaml.entry(KEY_VALUE(action->keys[x].trigger_flags));
				yaml.entry(KEY_VALUE(action->keys[x].modefier_flags));
			});

		m_actions.emplace_back(action);

	}
}
