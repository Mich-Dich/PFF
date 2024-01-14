#pragma once

#include "event.h"


namespace PFF {

	class key_event : public event {
	public:

		FORCEINLINE int32 get_keycode() const { return m_keycode; }
		EVENT_CLASS_CATEGORY(EC_Keyboard | EC_Input);
		EVENT_CLASS_TYPE(None);

	protected:
		key_event(int32 keyCode) : m_keycode(keyCode) {}

		int32 m_keycode;
	};


	class key_pressed_event : public key_event {
	public:

		key_pressed_event(int32 keyCode, int32 repeatCount) : key_event(keyCode), RepeatCount_m(repeatCount) {}

		FORCEINLINE int32 GetRepeatCount() const { return RepeatCount_m; }
		EVENT_CLASS_STRING("key pressed [code:" << m_keycode << ", repeats: " << RepeatCount_m << "]");
		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int32 RepeatCount_m;
	};


	class key_released_event : public key_event {
	public:

		key_released_event(int32 keyCode) : key_event(keyCode) {}

		EVENT_CLASS_STRING("key released [code: " << m_keycode << "]");
		EVENT_CLASS_TYPE(KeyReleased)
	};




	class MouseButtonPressedEvent : public key_event {
	public:

		MouseButtonPressedEvent(int32 button) : key_event(button) {}

		EVENT_CLASS_STRING("mouse button pressed: " << m_keycode);
		EVENT_CLASS_TYPE(MouseButtonPressed)
	};


	class MouseButtonReleasedEvent : public key_event {
	public:

		MouseButtonReleasedEvent(int32 button) : key_event(button) {}

		EVENT_CLASS_STRING("mouse button released: " << m_keycode);
		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}
