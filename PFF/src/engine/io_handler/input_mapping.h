#pragma once

#include "engine/io_handler/input_action.h"

#define REGISTER_INPUT_ACTION(action)	m_actions.push_back(action)

namespace PFF {

	class PFF_API input_mapping {

	public:

		input_mapping() {};
		~input_mapping() {};

		// DELETE_COPY(input_mapping);

		FORCEINLINE void register_action(input_action action) { m_actions.push_back(action); }

		std::vector<input_action>::iterator begin() { return m_actions.begin(); }
		std::vector<input_action>::iterator end() { return m_actions.end(); }

		std::vector<PFF::input_action> m_actions;
	private:

	};

}
