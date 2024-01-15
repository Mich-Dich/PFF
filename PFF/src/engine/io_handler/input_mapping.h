#pragma once

#include "engine/io_handler/input_action.h"

//@brief Registers an input_action and loades/updates its values from input.ini if the player has made modifications to the corresponding values.
// 		This ensures that the default values are retained unless the player explicitly changes them.
//@param [action] Reference to the input_action struct to be registered and configured.
#define REGISTER_INPUT_ACTION(action)	register_action(#action, action);

namespace PFF {

	class PFF_API input_mapping {

	public:

		input_mapping() {};
		~input_mapping() {};

		// DELETE_COPY(input_mapping);
		
		//@brief Registers an input_action and loades/updates its values from input.ini if the player has made modifications to the corresponding values.
		// 		This ensures that the default values are retained unless the player explicitly changes them.
		//@param [name] The name of the input action.
		//@param [action] Reference to the input_action struct to be registered and configured.
		void register_action(std::string name, input_action& action);

		std::vector<input_action>::iterator begin() { return m_actions.begin(); }
		std::vector<input_action>::iterator end() { return m_actions.end(); }

	private:
		std::vector<PFF::input_action> m_actions;

	};

}
