
#include "util/pffpch.h"
#include "game_map.h"

namespace PFF {



	game_map::game_map() {

		PFF_PROFILE_FUNCTION();

		CORE_LOG(Trace, "Init");
	}

	game_map::~game_map() {

		PFF_PROFILE_FUNCTION();

		CORE_LOG(Trace, "Shutdown");
	}

	game_object* game_map::create_empty_game_object() {

		PFF_PROFILE_FUNCTION();

		m_game_objects.push_back(game_object::create_game_object());
		return &m_game_objects[m_game_objects.size() - 1];
	}

	void game_map::remove_gameobject(u32 index) {
	}

}
