#pragma once

#include "engine/io_handler/input_mapping.h"

namespace PFF {

	class player_controller {
	
	public:

		player_controller();
		~player_controller();

		DELETE_COPY(player_controller);

		void update();

		void add_inpur_action(input_action* action);
		void update_input_action();

	private:

		input_mapping m_input_mapping;

	};

}
