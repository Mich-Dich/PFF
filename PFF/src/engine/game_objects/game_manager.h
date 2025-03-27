#pragma once

#include "engine/game_objects/player_controller.h"

namespace PFF {

	class game_manager {
	public:

		PFF_DELETE_COPY_CONSTRUCTOR(game_manager);

		static game_manager& instance() {
			static game_manager instance;
			return instance;
		}

		player_controller* p_player_controller{};
		//entity p_player{};

	private:

		game_manager();
		~game_manager();

	};

}
