
#include "util/pffpch.h"

#include "application.h"
#include "player_controller.h"

#include "engine/Events/Event.h"
#include "engine/Events/mouse_event.h"
#include "engine/Events/key_event.h"


// !! CAUTION !! not implemented yet
// defines the min diffrence between the input_value (in smooth input mode) and the target value
#define INPUT_ACTION_MODEFIER_MIN_DIF_BETWEEN_SMOOTH_INPUT_AND_TARGET		0.08


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

	bool player_controller::handle_key_events(key_event& event) {
		
		//CORE_LOG(Trace, event.get_keycode());
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
								buffer = event.m_key_state == key_state::press;

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
							// CORE_LOG(Info, "action value: " << bool_to_str(current_action.data.boolean));

						} break;

						// ================================================================= float & vec2 =================================================================
						case input_value::IV_float:
						case input_value::IV_vec2: {

							f32 buffer{};

							// =============================================== triggers ===============================================
							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_DOWN)
								buffer = (event.m_key_state != key_state::release) ? 1.0f : 0.0f;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_UP)
								buffer = (event.m_key_state == key_state::release) ? 1.0f : 0.0f;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_HOLD)
								buffer = (event.m_key_state == key_state::repeat) ? 1.0f : 0.0f;
								
							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_TAP) {

								// UNFINISHED
								buffer = (event.m_key_state == key_state::press) ? 1.0f : 0.0f;
							}

							// =============================================== modefiers ===============================================
							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_NEGATE)
								buffer *= -1.0f;

							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_SMOOTH_INTERP) {

							}


							// =============================================== case spacific saving of data ===============================================
							if (current_action.value == input_value::IV_float) {

								current_action.data.axis_1d = buffer;
								// CORE_LOG(Info, "action value: " << current_action.data.axis_1d);
							}

							else if (current_action.value == input_value::IV_vec2) {

								if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS)
									current_action.data.axis_2d.y = buffer;
								else
									current_action.data.axis_2d.x = buffer;

								CORE_LOG(Info, "action value: [X: " << current_action.data.axis_2d.x << " Y: " << current_action.data.axis_2d.y << "]");
							}


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
		
		//CORE_LOG(Trace, event);
		return false;
	}

}
