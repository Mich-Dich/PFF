#pragma once

#include "engine/io_handler/input_mapping.h"
#include "engine/Events/Event.h"
#include "engine/Events/mouse_event.h"
#include "engine/Events/key_event.h"

namespace PFF {

	class PFF_API player_controller {
	
	public:

		player_controller();
		~player_controller();

		DELETE_COPY(player_controller);

		virtual void init();

		void handle_event(event& event);

		input_mapping m_input_mapping;

	private:

		bool handle_key_events(key_event& event);
		bool handle_mouse_events(mouse_event& event);

	};

}
