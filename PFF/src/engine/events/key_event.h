#pragma once

#include <sstream>
#include <vector>

#include "util/util.h"
#include "Event.h"

namespace PFF {

	class PFF_API KeyEvent : public Event {

	public:
		inline int GetKeyCode() const { return KeyCode_m; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(int keyCode) : KeyCode_m(keyCode) {}

		int KeyCode_m;
	};

	class PFF_API KeyPressedEvent : public KeyEvent {

	public:
		KeyPressedEvent(int keyCode, int repeatCount) : KeyEvent(keyCode), RepeatCount_m(repeatCount) {}

		inline int GetRepeatCount() const { return RepeatCount_m; }

		std::string ToString() const override {

			std::stringstream ss;
			ss << "Event - KeyPressedEvent [Code:" << KeyCode_m << ", Repeats: " << RepeatCount_m << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int RepeatCount_m;
	};

	class PFF_API KeyReleasedEvent : public KeyEvent {

	public:
		KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

		std::string ToString() const override {

			std::stringstream ss;
			ss << "Event - KeyReleased [Code: " << KeyCode_m << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}