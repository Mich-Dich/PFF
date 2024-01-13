#pragma once


namespace {

	// --------------------------------------------------- General Action Structs ---------------------------------------------------

	#define INPUT_ACTION_TRIGGER_KEY_DOWN				BIT(0)
	#define INPUT_ACTION_TRIGGER_KEY_UP					BIT(1)
	#define INPUT_ACTION_TRIGGER_HOLD					BIT(2)
	#define INPUT_ACTION_TRIGGER_TAP					BIT(3)

	#define INPUT_ACTION_MODEFIER_NEGATE				BIT(0)
	#define INPUT_ACTION_MODEFIER_VEC2_NORMAL			BIT(1)
	#define INPUT_ACTION_MODEFIER_SMOOTH				BIT(2)

	#define MIN_DIF_BETWEEN_SMOOTH_INPUT_AND_TARGET		0.08
	#define INPUT_ACTION_REGISTER_NAME(var)				var.action_name = #var


	// --------------------------------------------------- Input Action ---------------------------------------------------
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

	typedef enum {
		IV_bool = 0,
		IV_float,
		IV_vec2
	} input_value;



	class input_action {

	public:

	private:

		std::string action_name;
		std::string description;
		bool triger_when_paused;
		input_value value;
		union value_type {
			bool boolean;
			float floating_point;
			glm::vec2 vector2D;
		};
		input::settings settings;

	};

}