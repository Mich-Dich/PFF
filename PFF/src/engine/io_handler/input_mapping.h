#pragma once

#include "engine/io_handler/input_action.h"

// if values of input_action are changed in editor code, it should save the new settings
#if defined(PFF_USING_EDITOR)							
	#define FORCE_OVERRIDE true
#else 										
	#define FORCE_OVERRIDE false
#endif						

//@brief Registers an input_action and loades/updates its values from input.ini if the player has made modifications to the corresponding values.
// 		This ensures that the default values are retained unless the player explicitly changes them.
//@param [name] The name of the input action.
//@param [action] Reference to the input_action struct to be registered and configured.
#define REGISTER_INPUT_ACTION(action)	action.set_name(#action);								\
										register_action(&action, FORCE_OVERRIDE);

namespace PFF {

	class input_mapping {
	public:

		input_mapping();
		~input_mapping() {};

		const size_t get_length() const { return m_actions.size(); }
		input_action* get_action(u32 index) { return m_actions[index]; }

		PFF_DELETE_COPY_CONSTRUCTOR(input_mapping);
		
		//@brief CAUTION - use REGISTER_INPUT_ACTION() instead 
		//@brief Registers an input_action and loades/updates its values from input.ini if the player has made modifications to the corresponding values.
		// 		This ensures that the default values are retained unless the player explicitly changes them.
		//@param [name] The name of the input action.
		//@param [action] Reference to the input_action struct to be registered and configured.
		void register_action(input_action* action, bool force_override);
		void register_action(input_action* action, bool force_override, std::filesystem::path path);

		std::vector<input_action*>::iterator begin() { return m_actions.begin(); }
		std::vector<input_action*>::iterator end() { return m_actions.end(); }

	private:
		
		void serialize_action(input_action* action, bool force_override, std::filesystem::path path);

		std::vector<input_action*> m_actions{};

	};

}
