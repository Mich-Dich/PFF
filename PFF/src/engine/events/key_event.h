#pragma once

#include "event.h"


namespace PFF {

	class key_event : public event {
	public:

		EVENT_CLASS_TYPE(None);
		EVENT_CLASS_CATEGORY(EC_Keyboard | EC_Input);
		FORCEINLINE key_code get_keycode() const { return m_keycode; }

		EVENT_CLASS_STRING("key: " << static_cast<u16>(m_keycode) << " state: " << static_cast<u16>(m_key_state));
		key_event(key_code keyCode, key_state key_state) : m_keycode(keyCode), m_key_state(key_state) {}

		key_code m_keycode;
		key_state m_key_state;
	
	};

}
