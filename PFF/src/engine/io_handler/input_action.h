#pragma once

// --------------------------------------------------- trigger and modefier flags ---------------------------------------------------

#define INPUT_ACTION_FLAGS_NONE						BIT(0)		// NEVER activate input
#define INPUT_ACTION_TRIGGER_NONE					BIT(0)		// NEVER activate input

// key trigger flags
#define INPUT_ACTION_TRIGGER_KEY_DOWN				BIT(1)		// activate input when key is pressed down (can repeat)
#define INPUT_ACTION_TRIGGER_KEY_UP					BIT(2)		// activate input when key NOT pressed (can repeat)
#define INPUT_ACTION_TRIGGER_KEY_HOLD				BIT(3)		// activate input when key down LONGER than [duration_in_sec] in input_action struct (can repeat)
#define INPUT_ACTION_TRIGGER_KEY_TAP				BIT(4)		// activate input when key down SHORTER than [duration_in_sec] in input_action struct (can repeat)
#define INPUT_ACTION_TRIGGER_KEY_MOVE_DOWN			BIT(5)		// activate input when starting to press a key (can NOT repeat)
#define INPUT_ACTION_TRIGGER_KEY_MOVE_UP			BIT(6)		// activate input when releasing a key (can NOT repeat)

// mouse trigger flags
#define INPUT_ACTION_TRIGGER_MOUSE_POSITIVE			BIT(10)
#define INPUT_ACTION_TRIGGER_MOUSE_NEGATIVE			BIT(11)
#define INPUT_ACTION_TRIGGER_MOUSE_POS_AND_NEG		BIT(12)

// modefier flags
#define INPUT_ACTION_MODEFIER_NONE					BIT(0)		// NEVER activate input
#define INPUT_ACTION_MODEFIER_NEGATE				BIT(1)
#define INPUT_ACTION_MODEFIER_USE_VEC_NORMAL		BIT(2)
#define INPUT_ACTION_MODEFIER_AXIS_1_NEGATIVE		BIT(3)
#define INPUT_ACTION_MODEFIER_AXIS_2				BIT(4)
#define INPUT_ACTION_MODEFIER_AXIS_2_NEGATIVE		BIT(5)
#define INPUT_ACTION_MODEFIER_AXIS_3				BIT(6)
#define INPUT_ACTION_MODEFIER_AXIS_3_NEGATIVE		BIT(7)
#define INPUT_ACTION_MODEFIER_AUTO_RESET			BIT(8)
#define INPUT_ACTION_MODEFIER_AUTO_RESET_ALL		BIT(9)

// !! CAUTION !! not implemented yet
#define INPUT_ACTION_MODEFIER_SMOOTH_INTERP			BIT(3)


// use name of var for internal string
#define INPUT_ACTION_REGISTER_NAME(var)				var.action_name = #var


#define INPUT_ACTION_BUFFER_NONE			0
#define INPUT_ACTION_BUFFER_POSITIVE		BIT(0)
#define INPUT_ACTION_BUFFER_NEGATIVE		BIT(1)

#define INPUT_ACTION_TYPE_BOOL				BIT(0)
#define INPUT_ACTION_TYPE_FLOAT				BIT(1)
#define INPUT_ACTION_TYPE_VEC_2				BIT(2)
#define INPUT_ACTION_TYPE_VEC_3				BIT(3)

namespace PFF {
	
	namespace input {

		enum class action_type {
			boolean,
			vec_1D,
			vec_2D,
			vec_3D,
		};

		struct key_binding_details {

			key_code key{};
			u16 trigger_flags{};
			u16 modefier_flags{};
			int16 active{};

			key_binding_details() : key(key_code::key_unknown), trigger_flags(0), modefier_flags(0) {};
			key_binding_details(key_code key, u16 trigger_flags = 0, u16 modefier_flags = 0)
				: key(key), trigger_flags(trigger_flags), modefier_flags(modefier_flags) {};
		};

		// --------------------------------------------------- Input Action ---------------------------------------------------
	}

	struct input_action {

		friend class player_controller;
		friend class input_mapping;

		union input_action_data {
			bool boolean;
			f32 vec_1D;
			glm::vec2 vec_2D;
			glm::vec3 vec_3D{};
		};

		std::string description{};
		bool triger_when_paused;
		u16 flags = 0;
		input::action_type value;
		input_action_data data;
		f32 duration_in_sec;
		std::vector<input::key_binding_details> keys_bindings;

		input_action(bool triger_when_paused = false, u16 flags = 0, input::action_type value = input::action_type::boolean, f32 duration_in_sec = 0.0f, std::vector<input::key_binding_details> keys_bindings = {})
			: triger_when_paused(triger_when_paused), flags(flags), value(value), duration_in_sec(duration_in_sec), keys_bindings(keys_bindings) {}

		const size_t get_length() const { return keys_bindings.size(); }
		input::key_binding_details* get_key(u32 index) { return &keys_bindings[index]; }

		/*
		template <typename T>
		T get_data() const {

			static_assert(std::is_same<T, bool>::value || std::is_same<T, f32>::value || std::is_same<T, glm::vec2>::value,
				"Unsupported data type");

			switch (value) {
			case PFF::input::action_type::_bool:
				return data.boolean;

			case PFF::input::action_type::_1D:
				return data.vec_1d;

			case PFF::input::action_type::_2D:
				return data.vec_2d;

			case PFF::input::action_type::_3D:
				return data.vec_3d;

			default:
				return T{};
			}
		}
		*/

		void set_name(std::string_view name) { this->name = name; }
		std::string_view get_name() const { return name; }

	private:
		std::string name{};
		input_action_data target;
		std::chrono::time_point < std::chrono::steady_clock> time_stamp{};

	};

}
