#pragma once

#include "engine/game_objects/player_controller.h"
#include "engine/game_objects/game_object.h"

namespace PFF{

	class game_manager {
	public:

		DELETE_COPY(game_manager);

		static game_manager& instance() {
			static game_manager instance;
			return instance;
		}

		player_controller* p_player_controller{};
		//game_object* p_player{};

	private:

		game_manager();
		~game_manager();

	};

}
