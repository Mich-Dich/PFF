#pragma once

#include "event.h"


namespace PFF {

	class PFF_API mouse_event : public event {
	public:

		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input)

	protected:
		mouse_event(int32 X, int32 Y) : m_X(X), m_Y(Y) {}

		int32 m_X, m_Y;
	};

	class PFF_API MouseMovedEvent : public mouse_event {
	public:

		MouseMovedEvent(f32 x, f32 y) : mouse_event(static_cast<u32>(x), static_cast<u32>(y)) {}

		FORCEINLINE int32 get_x() const { return m_X; }
		FORCEINLINE int32 get_y() const { return m_Y; }

		EVENT_CLASS_STRING("mouse moved [X: " << m_X << " Y: " << m_Y << "]");
		EVENT_CLASS_TYPE(MouseMoved);
		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input);

	};


	class MouseScrolledEvent : public mouse_event {
	public:

		MouseScrolledEvent(f32 xOffset, f32 yOffset) : mouse_event(static_cast<u32>(xOffset), static_cast<u32>(yOffset)) {}

		FORCEINLINE int32 get_x() const { return m_X; }
		FORCEINLINE int32 get_y() const { return m_Y; }

		EVENT_CLASS_STRING("mouse scrolled [X: " << m_X << " Y: " << m_Y << "]");
		EVENT_CLASS_TYPE(MouseScrolled);
		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input);

	};



}