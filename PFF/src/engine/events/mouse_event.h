#pragma once

#include "event.h"


namespace PFF {

	class PFF_API MouseMovedEvent : public event {

	public:
		MouseMovedEvent(f32 x, f32 y) : m_mouse_posX(x), m_mouse_posY(y) {}

		inline int32 GetX() const { return static_cast<int32>(m_mouse_posX); }
		inline int32 GetY() const { return static_cast<int32>(m_mouse_posY); }

		std::string to_string() const override {

			std::stringstream ss;
			ss << "Event - MouseMoved [X: " << m_mouse_posX << " Y: " << m_mouse_posY << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input)

	private:
		f32 m_mouse_posX, m_mouse_posY;
	};


	class PFF_API MouseScrolledEvent : public event {

	public:
		MouseScrolledEvent(f32 xOffset, f32 yOffset) : m_offsetX(xOffset), m_offsetY(yOffset) {}

		inline f32 GetXOffset() const { return m_offsetX; }
		inline f32 GetYOffset() const { return m_offsetY; }

		std::string to_string() const override {

			std::stringstream ss;
			ss << "Event - MouseScrolled [X: " << GetXOffset() << " Y: " << GetYOffset() << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input)

	private:
		f32 m_offsetX, m_offsetY;
	};


	class PFF_API MouseButtonEvent : public event {

	public:
		inline int32 GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EC_Mouse | EC_Input)

	protected:
		MouseButtonEvent(int32 button) : m_Button(button) {}

		int32 m_Button;
	};


	class PFF_API MouseButtonPressedEvent : public MouseButtonEvent {

	public:
		MouseButtonPressedEvent(int32 button) : MouseButtonEvent(button) {}

		std::string to_string() const override {

			std::stringstream ss;
			ss << "Event - MouseButtonPressed: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};


	class PFF_API MouseButtonReleasedEvent : public MouseButtonEvent {

	public:
		MouseButtonReleasedEvent(int32 button) : MouseButtonEvent(button) {}

		std::string to_string() const override {

			std::stringstream ss;
			ss << "Event - MouseButtonReleased: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}