#pragma once

#include "event.h"


namespace PFF {

	class key_event : public event {
	public:

		FORCEINLINE int32 get_keycode() const { return m_keycode; }
		EVENT_CLASS_CATEGORY(EC_Keyboard | EC_Input);
		EVENT_CLASS_TYPE(None);

		EVENT_CLASS_STRING("key: " << m_keycode << " state: " << static_cast<u16>(m_key_state));
		key_event(int32 keyCode, key_state key_state) : m_keycode(keyCode), m_key_state(key_state) {}

		int32 m_keycode;
		key_state m_key_state;
	
	};


}
