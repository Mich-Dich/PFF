#pragma once

#include "engine/game_objects/game_object.h"


namespace PFF {

	class game_map {

	public:

		game_map();
		~game_map();

		DELETE_COPY(game_map);

		FORCEINLINE std::vector<game_object>& get_all_game_objects() { return m_game_objects; }
		//FORCEINLINE game_object& get_game_objects_data(u32 index) const { return static_cast<game_object*>(*m_game_objects[index]); }		// TODO: copys at the moment, fix that

		game_object* create_empty_game_object();
		void remove_gameobject(u32 index);

	private:

		std::vector<game_object> m_game_objects{};

	};

}
