#pragma once

#include "engine/game_objects/game_object.h"


namespace PFF {

	class game_map {

	public:

		game_map();
		~game_map();

	private:

		std::vector<game_object> game_objects{};

	};

}
