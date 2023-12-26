#pragma once

#include <sstream>
#include <vector>

#include "engine/util.h"
#include "engine/types.h"
#include "Event.h"

namespace Gluttony {

	class PFF_API MouseMovedEvent : public Event {

	public:
		MouseMovedEvent(float x, float y) : MousePosX_m(x), MousePosY_m(y) {}

		inline int GetX() const { return MousePosX_m; }
		inline int GetY() const { return MousePosY_m; }

		std::string ToString() const override {

			std::stringstream ss;
			ss << "Event - MouseMoved [X: " << MousePosX_m << " Y: " << MousePosY_m << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float MousePosX_m, MousePosY_m;
	};

	class PFF_API MouseScrolledEvent : public Event {

	public:
		MouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override {

			std::stringstream ss;
			ss << "Event - MouseScrolled [X: " << GetXOffset() << " Y: " << GetYOffset() << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_XOffset, m_YOffset;
	};

	class PFF_API MouseButtonEvent : public Event {

	public:
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	protected:
		MouseButtonEvent(int button) : m_Button(button) {}

		int m_Button;
	};

	class PFF_API MouseButtonPressedEvent : public MouseButtonEvent {

	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override {

			std::stringstream ss;
			ss << "Event - MouseButtonPressed: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class PFF_API MouseButtonReleasedEvent : public MouseButtonEvent {

	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override {

			std::stringstream ss;
			ss << "Event - MouseButtonReleased: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}