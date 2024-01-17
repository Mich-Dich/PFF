
#include "util/pffpch.h"

#include "application.h"
#include "player_controller.h"

#include "engine/Events/Event.h"
#include "engine/Events/mouse_event.h"
#include "engine/Events/key_event.h"
#include "engine/layer/world_layer.h"


// !! CAUTION !! not implemented yet
// defines the min diffrence between the input_value (in smooth input mode) and the target value
#define INPUT_ACTION_MODEFIER_MIN_DIF_BETWEEN_SMOOTH_INPUT_AND_TARGET		0.08


namespace PFF {

	player_controller::player_controller() {
		
		init();
	}

	player_controller::~player_controller() {

		m_input_mapping.reset();
		CORE_LOG(Info, "Shutdown");
	}

	void player_controller::set_world_layer_ref(world_layer* world_layer) {

		m_world_layer = world_layer;
	}

	void player_controller::init() {
	}

	void player_controller::update(f32 delta) {
	}

	void player_controller::update_internal(f32 delta) {


		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {					// get input_action
			input_action* action = m_input_mapping->get_action(x);

			for (key_details key_details : action->keys) {							// get key in input_action				
				if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_SMOOTH_INTERP) {
				
				}
			}
		}
		
		update(delta);

		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {					// get input_action
			input_action* action = m_input_mapping->get_action(x);

			if (action->modefier_flags & INPUT_ACTION_MODEFIER_AUTO_RESET) {

				switch (action->value) {

					case input_value::_bool: {

						//action->data.boolean = false;
					} break;

					case input_value::_1D: {

						action->data._1D = 0;
					} break;
						
					case input_value::_2D: {

						action->data._2D = glm::vec2{ 0.0f };
					} break;

					case input_value::_3D: {

						action->data._3D = glm::vec3{ 0.0f };
					} break;

					default:
						break;
				}
			}
		}
	}

	void player_controller::handle_event(event& event) {

		if (event.is_in_category(EC_Input)) {

			event_dispatcher dispatcher(event);
			dispatcher.dispatch<key_event>(BIND_FN(player_controller::handle_key_events));
			dispatcher.dispatch<mouse_event>(BIND_FN(player_controller::handle_mouse_events));
		}
	}

	bool player_controller::handle_key_events(key_event& event) {
		
		//CORE_LOG(Trace, event.get_keycode());
		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {			// get input_action

			input_action* action = m_input_mapping->get_action(x);

			for (key_details key_details : action->keys) {				// get key in input_action
				if (key_details.key == event.get_keycode()) {					// check if I have an event for that key

					std::chrono::time_point<std::chrono::high_resolution_clock> time_now = std::chrono::high_resolution_clock::now();
					switch (action->value) {

						// ================================================================= bool =================================================================
						case input_value::_bool: {

							bool buffer = false;

							//LOG(Debug, "Test");
							// =============================================== triggers ===============================================
							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_DOWN)
								buffer = (event.m_key_state == key_state::press);

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_UP)
								buffer = (event.m_key_state == key_state::release);

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_HOLD)
								buffer = (event.m_key_state == key_state::repeat);

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_TAP) {


								action->time_stamp = time_now;
							}

							// =============================================== modefiers ===============================================
							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_NEGATE)
								buffer = !buffer;

							m_input_mapping->get_action(x)->data.boolean = buffer;
							//CORE_LOG(Info, "action value: " << bool_to_str(m_input_mapping->get_action(x)->data.boolean) << " key state: " << static_cast<int>(event.m_key_state));

						} break;

						// ================================================================= float & vec2 =================================================================
						case input_value::_1D:
						case input_value::_2D:
						case input_value::_3D: {

							f32 buffer{};

							// =============================================== triggers ===============================================
							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_DOWN)
								buffer = (event.m_key_state != key_state::release) ? 1.0f : 0.0f;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_UP)
								buffer = (event.m_key_state == key_state::release) ? 1.0f : 0.0f;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_HOLD)
								buffer = (event.m_key_state == key_state::repeat) ? 1.0f : 0.0f;
								
							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_KEY_TAP) {

								// UNFINISHED
								buffer = (event.m_key_state == key_state::press) ? 1.0f : 0.0f;
							}

							// =============================================== modefiers ===============================================
							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_NEGATE)
								buffer *= -1.0f;

							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_SMOOTH_INTERP) {

								// CORE_LOG(Info, "INPUT_ACTION_MODEFIER_SMOOTH_INTERP is not supported yet");
							}


							// =============================================== case spacific saving of data ===============================================
							if (action->value == input_value::_1D) {

								action->data._1D = buffer;
								// CORE_LOG(Info, "action value: " << action->data.axis_1d);
							}

							else if (action->value == input_value::_2D) {

								if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS)
									m_input_mapping->get_action(x)->data._2D.y = buffer;
								else
									m_input_mapping->get_action(x)->data._2D.x = buffer;

								// CORE_LOG(Info, "action value: [X: " << action->data.axis_2d.x << " Y: " << action->data.axis_2d.y << "]");
								// CORE_LOG(Info, "action value: [X: " << m_input_mapping->get_action(x)->data.axis_2d.x << " Y: " << m_input_mapping->get_action(x)->data.axis_2d.y << "]");
							}

							else if (action->value == input_value::_3D) {

								if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS)
									action->data._3D.y = buffer;
								else if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_VEC3_SECOND_AXIS)
									action->data._3D.z = buffer;
								else
									action->data._3D.x = buffer;

								//CORE_LOG(Info, "action value: [X: " << action->data.axis_2d.x << " Y: " << action->data.axis_2d.y << "]");
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
	/*
	#define INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG		BIT(4)
	#define INPUT_ACTION_TRIGGER_MOUSE_POSITIVE			BIT(5)
	#define INPUT_ACTION_TRIGGER_MOUSE_NEGATIVE			BIT(6)
	*/
	bool player_controller::handle_mouse_events(mouse_event& event) {

		CORE_ASSERT(m_input_mapping, "", "m_input_mapping is not set");
		// CORE_LOG(Trace, event << " key code: [" << std::setw(4) << static_cast<int32>(event.get_keycode()) << "] num of actions: " << m_input_mapping->get_length());

		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {					// get input_action
			input_action* action = m_input_mapping->get_action(x);

			for (key_details key_details : action->keys) {							// get key in input_action
				if (key_details.key == event.get_keycode()) {						// check if I have an event for that key

					std::chrono::time_point<std::chrono::high_resolution_clock> time_now = std::chrono::high_resolution_clock::now();
					switch (action->value) {

							// ================================================================= bool =================================================================
						case input_value::_bool: {

							bool buffer = false;
							f32 event_value = event.get_value();

							// =============================================== triggers ===============================================
							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG)
								buffer = (event_value != 0);

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_POSITIVE)
								buffer = (event_value > 0);

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_NEGATIVE)
								buffer = (event_value < 0);

							// =============================================== modefiers ===============================================
							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_NEGATE)
								buffer = !buffer;

							action->data.boolean = buffer;
							//CORE_LOG(Info, "action value: " << bool_to_str(action->data.boolean));

						} break;

						// ================================================================= float & vec2 =================================================================
						case input_value::_1D:
						case input_value::_2D:
						case input_value::_3D:{

							f32 buffer{};
							f32 event_value = event.get_value();

							// =============================================== triggers ===============================================
							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG)
								buffer += event_value;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_POSITIVE && (event_value > 0))
								buffer += event_value;

							if (key_details.trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_NEGATIVE && (event_value < 0))
								buffer += event_value;


							// =============================================== modefiers ===============================================
							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_NEGATE)
								buffer *= -1.0f;

							if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_SMOOTH_INTERP) {

								//CORE_LOG(Info, "INPUT_ACTION_MODEFIER_SMOOTH_INTERP is not supported yet");
							}


							// =============================================== case spacific saving of data ===============================================
							if (action->value == input_value::_1D) {

								action->data._1D = buffer;
								//CORE_LOG(Info, "action value: " << action->data.axis_1d);
							}

							else if (action->value == input_value::_2D) {

								if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS)
									action->data._2D.y = buffer;
								else
									action->data._2D.x = buffer;

								//CORE_LOG(Info, "action value: [X: " << action->data.axis_2d.x << " Y: " << action->data.axis_2d.y << "]");
							}

							else if (action->value == input_value::_3D) {

								if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS)
									action->data._3D.y = buffer;
								else if (key_details.modefier_flags & INPUT_ACTION_MODEFIER_VEC3_SECOND_AXIS)
									action->data._3D.z = buffer;
								else
									action->data._3D.x = buffer;

								//CORE_LOG(Info, "action value: [X: " << action->data.axis_2d.x << " Y: " << action->data.axis_2d.y << "]");
							}

						} break;

						default:
							break;
					}
				}
			}
		}

		return false;
	}

}
