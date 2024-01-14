#pragma once


namespace PFF {

	// --------------------------------------------------- trigger and modefier flags ---------------------------------------------------

	#define INPUT_ACTION_TRIGGER_KEY_DOWN				BIT(0)
	#define INPUT_ACTION_TRIGGER_KEY_UP					BIT(1)
	#define INPUT_ACTION_TRIGGER_HOLD					BIT(2)
	#define INPUT_ACTION_TRIGGER_TAP					BIT(3)

	#define INPUT_ACTION_MODEFIER_NEGATE				BIT(0)
	#define INPUT_ACTION_MODEFIER_VEC2_NORMAL			BIT(1)
	#define INPUT_ACTION_MODEFIER_SMOOTH				BIT(2)

	#define MIN_DIF_BETWEEN_SMOOTH_INPUT_AND_TARGET		0.08
	#define INPUT_ACTION_REGISTER_NAME(var)				var.action_name = #var

	/*
	namespace input {
		
		enum class key_state {

		};

	enum class input_value {
		_bool,
		_float,
		_vec2,
	};

	struct settings {
		float duration_in_sec;
		u32 trigger_flags;
		u32 modefiers_flags;
	};
}


typedef enum {
	KS_released,
	KS_pressed,
	KS_held
} Key_state;

*/

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

		key_details(key_code key, u16 trigger_flags = 0, u16 modefier_flags = 0) 
			: key(key), trigger_flags(trigger_flags), modefier_flags(modefier_flags) {};
	};

	struct input_action {

		std::string description;
		bool triger_when_paused;
		input_value value;
		union value_type {
			bool boolean;
			float floating_point;
			glm::vec2 vector2D;
		};
		std::vector<key_details> keys;

		std::vector<key_details>::iterator begin() { return keys.begin(); }
		std::vector<key_details>::iterator end() { return keys.end(); }
	};

}
