
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
	/*
	#define INPUT_ACTION_TRIGGER_KEY_DOWN				BIT(0)
	#define INPUT_ACTION_TRIGGER_KEY_UP					BIT(1)
	#define INPUT_ACTION_TRIGGER_KEY_HOLD				BIT(2)
	#define INPUT_ACTION_TRIGGER_KEY_TAP				BIT(3)

	#define INPUT_ACTION_TRIGGER_MOUSE_POSITIVE			BIT(4)
	#define INPUT_ACTION_TRIGGER_MOUSE_NEGATIVE			BIT(5)
	#define INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG		BIT(6)

	#define INPUT_ACTION_MODEFIER_NEGATE				BIT(0)
	#define INPUT_ACTION_MODEFIER_VEC2_NORMAL			BIT(1)
	#define INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS		BIT(2)
	#define INPUT_ACTION_MODEFIER_VEC3_SECOND_AXIS		BIT(3)
	#define INPUT_ACTION_MODEFIER_AUTO_RESET			BIT(4)
	*/
	void player_controller::update_internal(f32 delta) {

		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {				// get input_action
			input_action* action = m_input_mapping->get_action(x);

			if (action->modefier_flags & INPUT_ACTION_MODEFIER_AUTO_RESET || action->modefier_flags & INPUT_ACTION_MODEFIER_AUTO_RESET_ALL)
				action->data = {};

		}

		//CORE_LOG(Trace, event.get_keycode());
		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {
			input_action* action = m_input_mapping->get_action(x);				// get input_action

			if (action->modefier_flags & INPUT_ACTION_MODEFIER_SMOOTH_INTERP) {

			}

			for (u32 x = 0; x < action->get_length(); x++) {	
				key_details* key_details = action->get_key(x);					// get key

				f32 m_buffer = key_details->active;

				// =============================================== modefiers ===============================================
				if (key_details->modefier_flags & INPUT_ACTION_MODEFIER_NEGATE)
					m_buffer *= -1.f;

				if (key_details->modefier_flags & INPUT_ACTION_MODEFIER_SMOOTH_INTERP) {

					//CORE_LOG(Info, "INPUT_ACTION_MODEFIER_SMOOTH_INTERP is not supported yet");
				}

				switch (action->value) {

					// ==================================================================================================================================
					case input_value::_bool: {

						action->data.boolean = (m_buffer > 0.f);
					} break;

					// ==================================================================================================================================
					case input_value::_1D: {

						action->data._1D += m_buffer;
					} break;

					// ==================================================================================================================================
					case input_value::_2D: {

						if (key_details->modefier_flags & INPUT_ACTION_MODEFIER_AXIS_2)
							action->data._2D.y += m_buffer;
						else
							action->data._2D.x += m_buffer;
					} break;

					// ==================================================================================================================================
					case input_value::_3D: {

						if (key_details->modefier_flags & INPUT_ACTION_MODEFIER_AXIS_2)
							action->data._3D.y += m_buffer;
						else if (key_details->modefier_flags & INPUT_ACTION_MODEFIER_AXIS_3)
							action->data._3D.z += m_buffer;
						else
							action->data._3D.x += m_buffer;

					} break;

					default:
						break;
				}
			}

			if (action->modefier_flags & INPUT_ACTION_MODEFIER_VEC_NORMAL) {
				switch (action->value) {

					// ==================================================================================================================================
					case input_value::_1D: {
						action->data._1D = std::clamp(action->data._1D, -1.f, 1.f);
					} break;

					// ==================================================================================================================================
					case input_value::_2D: {
						if (glm::dot(action->data._2D, action->data._2D) > std::numeric_limits<f32>::epsilon()) {

							auto m_buffer = action->data._2D;
							action->data._2D = glm::normalize(m_buffer);
						}
					} break;

					// ==================================================================================================================================
					case input_value::_3D: {
						if (glm::dot(action->data._3D, action->data._3D) > std::numeric_limits<f32>::epsilon()) {

							auto m_buffer = action->data._3D;
							action->data._3D = glm::normalize(m_buffer);
							//LOG(Trace, vec_to_str(action->data._3D));
						}

					} break;

					default:
						break;
				}
			}

		}

		update(delta);

		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {				// get input_action
			input_action* action = m_input_mapping->get_action(x);

			if (action->modefier_flags & INPUT_ACTION_MODEFIER_AUTO_RESET_ALL) {

				for (u32 x = 0; x < action->get_length(); x++) {
					key_details* key_details = action->get_key(x);				// get key

					key_details->active = {};
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
		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {				// get input_action
			input_action* action = m_input_mapping->get_action(x);

			for (u32 x = 0; x < action->get_length(); x++) {					// get input_action
				key_details* key_details = action->get_key(x);

				if (key_details->key == event.get_keycode()) {					// check if I have an event for that key


					std::chrono::time_point<std::chrono::high_resolution_clock> time_now = std::chrono::high_resolution_clock::now();
					bool m_buffer = false;

					//LOG(Debug, "Test");
					// =============================================== triggers ===============================================
					if (key_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_DOWN)
						m_buffer = (event.m_key_state != key_state::release);

					if (key_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_UP)
						m_buffer = (event.m_key_state == key_state::release);

					if (key_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_HOLD)
						m_buffer = (event.m_key_state == key_state::repeat);

					if (key_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_TAP) {

						CORE_LOG(Warn, "INPUT_ACTION_TRIGGER_KEY_TAP - not implemented yet")
						action->time_stamp = time_now;
					}
					
					key_details->active = (m_buffer) ? 1 : 0;
					//LOG(Debug, "Key: " << static_cast<int32>(key_details->key) << " is " << bool_to_str(key_details->active));
				}
			}
		}
		return true;
	}


	bool player_controller::handle_mouse_events(mouse_event& event) {

		CORE_ASSERT(m_input_mapping, "", "m_input_mapping is not set");
		// CORE_LOG(Trace, event << " key code: [" << std::setw(4) << static_cast<int32>(event.get_keycode()) << "] num of actions: " << m_input_mapping->get_length());

		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {			// get input_action
			input_action* action = m_input_mapping->get_action(x);

			for (u32 x = 0; x < action->get_length(); x++) {				// get input_action
				key_details* key_details = action->get_key(x);

				if (key_details->key == event.get_keycode()) {						// check if I have an event for that key

					std::chrono::time_point<std::chrono::high_resolution_clock> time_now = std::chrono::high_resolution_clock::now();

					bool m_buffer = false;
					f32 event_value = event.get_value();

					// =============================================== triggers ===============================================
					if (key_details->trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG)
						m_buffer = (event_value != 0);

					if (key_details->trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_POSITIVE)
						m_buffer = (event_value > 0);

					if (key_details->trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_NEGATIVE)
						m_buffer = (event_value < 0);

					key_details->active = (m_buffer) ? static_cast<int16>(event_value) : 0;
					//CORE_LOG(Info, "action value: " << bool_to_str(action->data.boolean));

				}
			}
		}

		return false;
	}

}
