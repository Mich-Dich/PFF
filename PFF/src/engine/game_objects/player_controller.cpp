
#include "util/pffpch.h"

#include "player_controller.h"
#include "engine/Events/mouse_event.h"
#include "engine/Events/key_event.h"
#include "application.h"

namespace PFF {



	player_controller::player_controller() {
		
		init();
	}

	player_controller::~player_controller() {
	}

	void player_controller::init() {
	}

	void player_controller::handle_event(event& event) {

		if (event.is_in_category(EC_Input)) {

			event_dispatcher dispatcher(event);
			dispatcher.dispatch<key_event>(BIND_FN(player_controller::handle_key_events));

			dispatcher.dispatch<MouseMovedEvent>(BIND_FN(player_controller::handle_mouse_events));
			dispatcher.dispatch<MouseScrolledEvent>(BIND_FN(player_controller::handle_mouse_events));
		}
	}
	/*
INPUT_ACTION_TRIGGER_KEY_DOWN				BIT(0)
INPUT_ACTION_TRIGGER_KEY_UP					BIT(1)
INPUT_ACTION_TRIGGER_HOLD					BIT(2)
INPUT_ACTION_TRIGGER_TAP					BIT(3)

INPUT_ACTION_MODEFIER_NEGATE				BIT(0)
INPUT_ACTION_MODEFIER_SMOOTH_INTERP			BIT(1)
INPUT_ACTION_MODEFIER_VEC2_NORMAL			BIT(2)
*/

	bool player_controller::handle_key_events(key_event& event) {
		
		CORE_LOG(Trace, event.get_keycode());

		for (input_action& current_action : m_input_mapping) {						// get input_action
			for (key_details key_details : current_action.keys) {					// get key in input_action
				if (static_cast<u32>(key_details.key) == event.get_keycode()) {		// check if I have an event for that key

					std::chrono::time_point<std::chrono::high_resolution_clock> time_now = std::chrono::high_resolution_clock::now();
					switch (current_action.value) {

						// ================================================================= bool =================================================================
						case input_value::IV_bool: {

							bool buffer = false;

							// =============================================== triggers ===============================================
							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_DOWN)
								buffer = event.m_key_state != key_state::release;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_UP)
								buffer = event.m_key_state == key_state::release;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_HOLD)
								buffer = event.m_key_state == key_state::repeat;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_TAP) {


								current_action.time_stamp = time_now;
							}

							// =============================================== modefiers ===============================================
							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_NEGATE)
								buffer = !buffer;

							current_action.data.boolean = buffer;
							LOG(Info, "action value: " << bool_to_str(current_action.data.boolean));

						} break;

						// ================================================================= float =================================================================
						case input_value::IV_float: {

							f32 buffer = 0;

							// =============================================== triggers ===============================================
							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_DOWN)
								buffer = (event.m_key_state != key_state::release) ? 1.0f : 0.0f;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_UP)
								buffer = (event.m_key_state == key_state::release) ? 1.0f : 0.0f;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_HOLD)
								buffer = (event.m_key_state == key_state::repeat) ? 1.0f : 0.0f;
								
							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_TAP) {

							}

							// =============================================== modefiers ===============================================
							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_NEGATE)
								buffer *= -1.0f;

							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_SMOOTH_INTERP) {

							}

							current_action.data.axis_1d = buffer;
							LOG(Info, "action value: " << current_action.data.axis_1d);

						} break;

						// ================================================================= vec2 =================================================================
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
		
		LOG(Trace, event);
		return false;
	}

}
