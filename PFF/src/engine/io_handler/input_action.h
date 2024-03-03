#pragma once


namespace PFF {

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

	// --------------------------------------------------- Input Action ---------------------------------------------------

	enum class input_value {
		boolean,
		vec_1D,
		vec_2D,
		vec_3D,
	};

	struct key_details {
		key_code key{};
		u16 trigger_flags{};
		u16 modefier_flags{};
		int16 active{};

		key_details() : key(key_code::key_unknown), trigger_flags(0), modefier_flags(0) {};
		key_details(key_code key, u16 trigger_flags = 0, u16 modefier_flags = 0) 
			: key(key), trigger_flags(trigger_flags), modefier_flags(modefier_flags) {};
	};

	#define INPUT_ACTION_BUFFER_NONE			0
	#define INPUT_ACTION_BUFFER_POSITIVE		BIT(0)
	#define INPUT_ACTION_BUFFER_NEGATIVE		BIT(1)

	#define INPUT_ACTION_TYPE_BOOL				BIT(0)
	#define INPUT_ACTION_TYPE_FLOAT				BIT(1)
	#define INPUT_ACTION_TYPE_VEC_2				BIT(2)
	#define INPUT_ACTION_TYPE_VEC_3				BIT(3)

	struct input_action {

		friend class player_controller;
		friend class input_mapping;

		union input_action_data {
			bool boolean;
			f32 _1D;
			glm::vec2 _2D;
			glm::vec3 _3D{};
		};

		// public
		std::string description{};
		bool triger_when_paused;
		u16 flags = 0;
		input_value value;
		input_action_data data;
		f32 duration_in_sec;
		std::vector<key_details> keys;

		//private


		input_action(bool triger_when_paused = false, u16 flags = 0, input_value value = input_value::boolean, f32 duration_in_sec = 0.0f, std::vector<key_details> keys = {})
			: triger_when_paused(triger_when_paused), flags(flags), value(value), duration_in_sec(duration_in_sec), keys(keys) {}

		const size_t get_length() const { return keys.size(); }
		key_details* get_key(u32 index) { return &keys[index]; }


		template <typename T>
		T get_data() const {

			static_assert(std::is_same<T, bool>::value || std::is_same<T, f32>::value || std::is_same<T, glm::vec2>::value,
				"Unsupported data type");

			switch (value) {
			case PFF::input_value::_bool:
				return data.boolean;

			case PFF::input_value::_1D:
				return data.vec_1d;

			case PFF::input_value::_2D:
				return data.vec_2d;

			case PFF::input_value::_3D:
				return data.vec_3d;

			default:
				return T{};
			}
		}
		
		void set_name(std::string_view name) { this->name = name; }
		std::string_view get_name() const { return name; }

	private:
		std::string name{};
		input_action_data target;
		std::chrono::time_point < std::chrono::steady_clock> time_stamp{};

	};

}


/*
struct input_action {

	input_value value;
	union {
		bool boolean;
		f32 _1D;
		glm::vec2 _2D;
		glm::vec3 _3D{};
	} data;

	template <typename T>
	data get_data() const {

		static_assert(std::is_same<T, bool>::value || std::is_same<T, f32>::value || std::is_same<T, glm::vec2>::value,
			"Unsupported data type");

		switch (value) {
		case PFF::input_value::_bool:
			return data.boolean;

		case PFF::input_value::_1D:
			return data._1d;

		case PFF::input_value::_2D:
			return data._2d;

		case PFF::input_value::_3D:
			return data._3d;

		default:
			return T{};
		}
	}

};
*/

/*
enum class input_value {
	_bool,
	_1D,
	_2D,
	_3D,
};

struct input_action {

	union input_action_data {
		bool boolean;
		f32 _1D;
		glm::vec2 _2D;
		glm::vec3 _3D{};
	};

	input_value value;
	input_action_data data;

};

 I have a struct with a union inside. How can I make a get_data() function that can just be called like this:
 example_input_action->get_data()		// this should return a [bool] if(value == input_value::_bool)

 Not like this:
 example_input_action->get_data<bool>()
 example_input_action->get_data_bool()

 */