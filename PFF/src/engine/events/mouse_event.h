#pragma once

#include "event.h"


namespace PFF {

	class PFF_API mouse_event : public event {
	public:

		EVENT_CLASS_TYPE(MouseMoved);
		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input);
		FORCEINLINE key_code get_keycode() const { return m_keycode; }

		FORCEINLINE f32 get_value() const { return m_delta; }
		EVENT_CLASS_STRING("mouse moved [" << std::setw(4) << static_cast<f32>(m_delta) << "]");

		mouse_event(key_code keyCode, f32 delta) : m_keycode(keyCode), m_delta(delta) {}

	private:
		f32 m_delta;
		key_code m_keycode;
	};

}
