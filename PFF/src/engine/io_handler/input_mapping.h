#pragma once

#include "engine/io_handler/input_action.h"

namespace PFF {

	class PFF_API input_mapping {

	public:

		input_mapping() {};
		~input_mapping() {};

		// DELETE_COPY(input_mapping);

		// bool input_register_action_bool(key_code key, input_action* action);

		std::vector<input_action>::iterator begin() { return m_actions.begin(); }
		std::vector<input_action>::iterator end() { return m_actions.end(); }

		std::vector<PFF::input_action> m_actions;
	private:

	};

}
