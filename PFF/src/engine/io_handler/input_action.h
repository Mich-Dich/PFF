#pragma once


namespace PFF {

	// --------------------------------------------------- trigger and modefier flags ---------------------------------------------------

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

	// !! CAUTION !! not implemented yet
	#define INPUT_ACTION_MODEFIER_SMOOTH_INTERP			BIT(3)

	#define INPUT_ACTION_REGISTER_NAME(var)				var.action_name = #var

	// --------------------------------------------------- Input Action ---------------------------------------------------

	enum class input_value {
		_bool,
		_1D,
		_2D,
		_3D,
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

		std::string name{};
		std::string description{};
		bool triger_when_paused;
		u16 modefier_flags = 0;
		input_value value;
		union {
			bool boolean;
			f32 _1D;
			glm::vec2 _2D;
			glm::vec3 _3D{};
		} data;
		std::chrono::time_point<std::chrono::high_resolution_clock> time_stamp{};
		f32 duration_in_sec;
		std::vector<key_details> keys;

		input_action(bool triger_when_paused = false, u16 modefier_flags = 0, input_value value = input_value::_bool, f32 duration_in_sec = 0.0f, std::vector<key_details> keys = {})
			: triger_when_paused(triger_when_paused), modefier_flags(modefier_flags), value(value), duration_in_sec(duration_in_sec), keys(keys) {}

		template <typename T>
		FORCEINLINE T get_data() const {
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

}