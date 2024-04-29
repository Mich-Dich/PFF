
#include "util/pffpch.h"

#include "application.h"
#include "player_controller.h"

#include "engine/Events/Event.h"
#include "engine/Events/mouse_event.h"
#include "engine/Events/key_event.h"
#include "engine/layer/world_layer.h"


// !! CAUTION !! not implemented yet
// defines the min diffrence between the input::action_type (in smooth input mode) and the target value
#define INPUT_ACTION_MODEFIER_MIN_DIF_BETWEEN_SMOOTH_INPUT_AND_TARGET		0.08


namespace PFF {

	player_controller::player_controller() {

		PFF_PROFILE_FUNCTION();

		init();
	}

	player_controller::~player_controller() {

		PFF_PROFILE_FUNCTION();

		m_input_mapping.reset();
		CORE_LOG(Trace, "Shutdown");
	}

	void player_controller::set_world_layer_ref(world_layer* world_layer) {

		PFF_PROFILE_FUNCTION();

		m_world_layer = world_layer;
	}

	void player_controller::init() { }

	void player_controller::update(f32 delta) { }

	/*
	#define INPUT_ACTION_TRIGGER_KEY_DOWN				BIT(0)		// activate input when key is pressed down (can repeat)
	#define INPUT_ACTION_TRIGGER_KEY_UP					BIT(1)		// activate input when key NOT pressed (can repeat)
	#define INPUT_ACTION_TRIGGER_KEY_HOLD				BIT(2)		// activate input when key down LONGER than [duration_in_sec] in input_action struct (can repeat)
	#define INPUT_ACTION_TRIGGER_KEY_TAP				BIT(3)		// activate input when key down SHORTER than [duration_in_sec] in input_action struct (can repeat)
	#define INPUT_ACTION_TRIGGER_KEY_MOVE_DOWN			BIT(4)		// activate input when starting to press a key (can NOT repeat)
	#define INPUT_ACTION_TRIGGER_KEY_MOVE_UP			BIT(5)		// activate input when releasing a key (can NOT repeat)

	#define INPUT_ACTION_TRIGGER_MOUSE_POSITIVE			BIT(10)
	#define INPUT_ACTION_TRIGGER_MOUSE_NEGATIVE			BIT(11)
	#define INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG		BIT(12)

	#define INPUT_ACTION_MODEFIER_NEGATE				BIT(0)
	#define INPUT_ACTION_MODEFIER_VEC2_NORMAL			BIT(1)
	#define INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS		BIT(2)
	#define INPUT_ACTION_MODEFIER_VEC3_SECOND_AXIS		BIT(3)
	#define INPUT_ACTION_MODEFIER_AUTO_RESET			BIT(4)
	*/

	void player_controller::update_internal(f32 delta) {

		PFF_PROFILE_FUNCTION();

		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {				// reset action->data if needed
			input_action* action = m_input_mapping->get_action(x);

			if (action->flags & INPUT_ACTION_MODEFIER_AUTO_RESET || action->flags & INPUT_ACTION_MODEFIER_AUTO_RESET_ALL)
				action->data = {};

		}

		//CORE_LOG(Trace, event.get_keycode());
		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {
			input_action* action = m_input_mapping->get_action(x);				// get input_action

			if (action->flags & INPUT_ACTION_MODEFIER_SMOOTH_INTERP) {

			}

			for (u32 x = 0; x < action->get_length(); x++) {	
				input::key_binding_details* key_binding_details = action->get_key(x);					// get key

				f32 m_buffer = key_binding_details->active;

				// =============================================== modefiers ===============================================
				if (key_binding_details->modefier_flags & INPUT_ACTION_MODEFIER_NEGATE)
					m_buffer *= -1.f;

				if (key_binding_details->modefier_flags & INPUT_ACTION_MODEFIER_SMOOTH_INTERP) {

					//CORE_LOG(Info, "INPUT_ACTION_MODEFIER_SMOOTH_INTERP is not supported yet");
				}

				switch (action->value) {

					// ==================================================================================================================================
					case input::action_type::boolean: {

						action->data.boolean = (m_buffer > 0.f);

						//if (key_binding_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_MOVE_DOWN) {}

					} break;

					// ==================================================================================================================================
					case input::action_type::vec_1D: {

						action->data.vec_1D += m_buffer;
					} break;

					// ==================================================================================================================================
					case input::action_type::vec_2D: {

						if (key_binding_details->modefier_flags & INPUT_ACTION_MODEFIER_AXIS_2)
							action->data.vec_2D.y += m_buffer;
						else
							action->data.vec_2D.x += m_buffer;
					} break;

					// ==================================================================================================================================
					case input::action_type::vec_3D: {

						if (key_binding_details->modefier_flags & INPUT_ACTION_MODEFIER_AXIS_2)
							action->data.vec_3D.y += m_buffer;
						else if (key_binding_details->modefier_flags & INPUT_ACTION_MODEFIER_AXIS_3)
							action->data.vec_3D.z += m_buffer;
						else
							action->data.vec_3D.x += m_buffer;

					} break;

					default:
						break;
				}
			}

			if (action->flags & INPUT_ACTION_MODEFIER_USE_VEC_NORMAL) {
				switch (action->value) {

					// ==================================================================================================================================
					case input::action_type::vec_1D: {
						action->data.vec_1D = std::clamp(action->data.vec_1D, -1.f, 1.f);
					} break;

					// ==================================================================================================================================
					case input::action_type::vec_2D: {
						if (glm::dot(action->data.vec_2D, action->data.vec_2D) > std::numeric_limits<f32>::epsilon())
							action->data.vec_2D = glm::normalize(action->data.vec_2D);

					} break;

					// ==================================================================================================================================
					case input::action_type::vec_3D: {
						if (glm::dot(action->data.vec_3D, action->data.vec_3D) > std::numeric_limits<f32>::epsilon())
							action->data.vec_3D = glm::normalize(action->data.vec_3D);

					} break;

					default:
						break;
				}
			}

		}

		update(delta);

		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {				// get input_action
			input_action* action = m_input_mapping->get_action(x);

			if (action->flags & INPUT_ACTION_MODEFIER_AUTO_RESET_ALL) {

				for (u32 x = 0; x < action->get_length(); x++) {
					input::key_binding_details* key_binding_details = action->get_key(x);				// get key

					key_binding_details->active = {};
				}
			}
		}
	}

	void player_controller::handle_event(event& event) {

		PFF_PROFILE_FUNCTION();

		if (event.is_in_category(EC_Input)) {

			event_dispatcher dispatcher(event);
			dispatcher.dispatch<key_event>(BIND_FN(player_controller::handle_key_events));
			dispatcher.dispatch<mouse_event>(BIND_FN(player_controller::handle_mouse_events));
		}
	}

	bool player_controller::handle_key_events(key_event& event) {

		PFF_PROFILE_FUNCTION();

		//CORE_LOG(Trace, event.get_keycode());
		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {				// get input_action
			input_action* action = m_input_mapping->get_action(x);

			for (u32 x = 0; x < action->get_length(); x++) {					// get input_action
				input::key_binding_details* key_binding_details = action->get_key(x);

				if (key_binding_details->key != event.get_keycode()) 					// check if I have an event for that key
					continue;

				std::chrono::time_point<std::chrono::steady_clock> time_now = std::chrono::steady_clock::now();
				bool m_buffer = false;


				// =============================================== triggers ===============================================
				// secondary if check to avoid overriding
				// key_binding can have multiple triggers && only one could be active
				do {
					if (key_binding_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_DOWN)
						m_buffer = (event.m_key_state != key_state::release);

					if (m_buffer)
						break;

					if (key_binding_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_UP)
						m_buffer = (event.m_key_state == key_state::release);

					if (m_buffer)
						break;

					if (key_binding_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_HOLD)
						m_buffer = (event.m_key_state == key_state::repeat);

					if (m_buffer)
						break;

					if (key_binding_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_TAP) {

						CORE_LOG(Warn, "INPUT_ACTION_TRIGGER_KEY_TAP - not implemented yet");
						action->time_stamp = time_now;
					}

					if (m_buffer)
						break;

					if (key_binding_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_MOVE_DOWN) {

						m_buffer = (event.m_key_state == key_state::press && key_binding_details->active == false);
						// CORE_LOG(Info, "Current buffer: " << m_buffer << " [" << util::bool_to_str(event.m_key_state == key_state::press) << "/" << util::bool_to_str(key_binding_details->active == false) << "]");
					}

					if (m_buffer)
						break;

					if (key_binding_details->trigger_flags & INPUT_ACTION_TRIGGER_KEY_MOVE_UP)
						m_buffer = (event.m_key_state == key_state::repeat);

				} while (false);


				key_binding_details->active = (m_buffer) ? 1 : 0;
				//LOG(Debug, "Key: " << static_cast<int32>(key_binding_details->key) << " is " << bool_to_str(key_binding_details->active));
			}
		}
		return true;
	}

	bool player_controller::handle_mouse_events(mouse_event& event) {

		PFF_PROFILE_FUNCTION();

		CORE_ASSERT(m_input_mapping, "", "m_input_mapping is not set");
		// CORE_LOG(Trace, event << " key code: [" << std::setw(4) << static_cast<int32>(event.get_keycode()) << "] num of actions: " << m_input_mapping->get_length());

		for (u32 x = 0; x < m_input_mapping->get_length(); x++) {			// get input_action
			input_action* action = m_input_mapping->get_action(x);

			for (u32 x = 0; x < action->get_length(); x++) {				// get input_action
				input::key_binding_details* key_binding_details = action->get_key(x);

				if (key_binding_details->key == event.get_keycode()) {						// check if I have an event for that key

					std::chrono::time_point<std::chrono::steady_clock> time_now = std::chrono::steady_clock::now();

					bool m_buffer = false;
					f32 event_value = event.get_value();

					// =============================================== triggers ===============================================
					// secondary if check to avoid overriding
					// key_binding can have multiple triggers && only one could be active
					do {

						if (key_binding_details->trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG)
							m_buffer = (event_value != 0);

						if (m_buffer)
							break;

						if (key_binding_details->trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_POSITIVE)
							m_buffer = (event_value > 0);

						if (m_buffer)
							break;

						if (key_binding_details->trigger_flags & INPUT_ACTION_TRIGGER_MOUSE_NEGATIVE)
							m_buffer = (event_value < 0);

					} while (false);

					key_binding_details->active = (m_buffer) ? static_cast<int16>(event_value) : 0;
					//CORE_LOG(Info, "action value: " << bool_to_str(action->data.boolean));

				}
			}
		}

		return false;
	}

}
