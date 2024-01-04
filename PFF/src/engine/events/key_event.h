#pragma once

#include "event.h"


namespace PFF {

	class PFF_API key_event : public event {

	public:
		inline int32 get_keycode() const { return m_keycode; }

		EVENT_CLASS_CATEGORY(EC_Keyboard | EC_Input)

	protected:
		key_event(int32 keyCode) : m_keycode(keyCode) {}

		int32 m_keycode;
	};

	class PFF_API key_pressed_event : public key_event {

	public:
		key_pressed_event(int32 keyCode, int32 repeatCount) : key_event(keyCode), RepeatCount_m(repeatCount) {}

		inline int32 GetRepeatCount() const { return RepeatCount_m; }

		std::string to_string() const override {

			std::stringstream ss;
			ss << "Event - KeyPressedEvent [Code:" << m_keycode << ", Repeats: " << RepeatCount_m << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int32 RepeatCount_m;
	};

	class PFF_API key_released_event : public key_event {

	public:
		key_released_event(int32 keyCode) : key_event(keyCode) {}

		std::string to_string() const override {

			std::stringstream ss;
			ss << "Event - KeyReleased [Code: " << m_keycode << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}