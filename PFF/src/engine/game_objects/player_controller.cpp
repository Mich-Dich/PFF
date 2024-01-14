
#include "util/pffpch.h"

#include "player_controller.h"
#include "engine/Events/mouse_event.h"
#include "engine/Events/key_event.h"


namespace PFF {



	player_controller::player_controller() {
	}

	player_controller::~player_controller() {
	}

	void player_controller::init() {
	}

	void player_controller::handle_event(event& event) {

		if (event.is_in_category(EC_Input)) {

			event_dispatcher dispatcher(event);
			dispatcher.dispatch<key_pressed_event>(BIND_FN(player_controller::handle_key_events));
			dispatcher.dispatch<key_released_event>(BIND_FN(player_controller::handle_key_events));
			dispatcher.dispatch<MouseButtonPressedEvent>(BIND_FN(player_controller::handle_key_events));
			dispatcher.dispatch<MouseButtonReleasedEvent>(BIND_FN(player_controller::handle_key_events));

			dispatcher.dispatch<MouseMovedEvent>(BIND_FN(player_controller::handle_mouse_events));
			dispatcher.dispatch<MouseScrolledEvent>(BIND_FN(player_controller::handle_mouse_events));
		}
	}

	bool player_controller::handle_key_events(key_event& event) {
		
		LOG(Trace, event.get_keycode());
		for (input_action current_action : m_input_mapping) {

			for (key_details key : current_action.keys) {

				if (static_cast<u32>(key.key) == event.get_keycode()) {

					switch (current_action.value) {

						case input_value::IV_bool: {

						} break;
					
						case input_value::IV_float: {

						} break;
					
						case input_value::IV_vec2: {

						} break;

						default:
							break;
					}
				}
			}
		}
		return true;
	}

	bool player_controller::handle_mouse_events(mouse_event& event) {
		LOG(Info, event);
		return false;
	}

	void player_controller::add_inpur_action(input_action* action) {
	}

	void player_controller::update_input_action() {
	}

}
