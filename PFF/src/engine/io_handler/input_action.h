#pragma once


namespace PFF {

	// --------------------------------------------------- trigger and modefier flags ---------------------------------------------------

	#define INPUT_ACTION_TRIGGER_KEY_DOWN				BIT(0)
	#define INPUT_ACTION_TRIGGER_KEY_UP					BIT(1)
	#define INPUT_ACTION_TRIGGER_KEY_HOLD				BIT(2)
	#define INPUT_ACTION_TRIGGER_KEY_TAP				BIT(3)

	#define INPUT_ACTION_TRIGGER_MOUSE_ANY_EVENT		BIT(4)
	#define INPUT_ACTION_TRIGGER_MOUSE_POSITIVE			BIT(5)
	#define INPUT_ACTION_TRIGGER_MOUSE_NEGATIVE			BIT(6)

	#define INPUT_ACTION_MODEFIER_NEGATE				BIT(0)
	#define INPUT_ACTION_MODEFIER_VEC2_NORMAL			BIT(1)
	#define INPUT_ACTION_MODEFIER_VEC2_SECOND_AXIS		BIT(2)

	// !! CAUTION !! not implemented yet
	#define INPUT_ACTION_MODEFIER_SMOOTH_INTERP			BIT(3)

	#define INPUT_ACTION_REGISTER_NAME(var)				var.action_name = #var

	// --------------------------------------------------- Input Action ---------------------------------------------------

	enum class input_value {
		IV_bool,
		IV_float,
		IV_vec2
	};

	struct key_details {
		key_code key;
		u16 trigger_flags;
		u16 modefier_flags;

		key_details() : key(key_code::key_unknown), trigger_flags(0), modefier_flags(0) {};
		key_details(key_code key, u16 trigger_flags = 0, u16 modefier_flags = 0) 
			: key(key), trigger_flags(trigger_flags), modefier_flags(modefier_flags) {};
	};

	struct input_action {

		std::string name;
		std::string description;
		bool triger_when_paused;
		input_value value;
		union {
			bool boolean;
			f32 axis_1d;
			glm::vec2 axis_2d{};
		} data;
		std::chrono::time_point<std::chrono::high_resolution_clock> time_stamp;
		f32 duration_in_sec;
		std::vector<key_details> keys;

		input_action(bool triger_when_paused = false, input_value value = input_value::IV_bool, std::vector<key_details> keys = {})
			: triger_when_paused(triger_when_paused), value(value), keys(keys) {}

	};

}
