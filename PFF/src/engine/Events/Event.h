#pragma once

#include <sstream>
#include <vector>

#include "util/util.h"

namespace PFF {

	// FIXME: Event are currently blocking => Fix that!

	enum class EventType {
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory {
		None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};

#define EVENT_CLASS_TYPE(type)				static EventType GetStaticType() { return EventType::##type; }					\
											virtual EventType GetEventType() const override { return GetStaticType(); }		\
											virtual const char* GetName() const override { return #type; }

#define	EVENT_CLASS_CATEGORY(category)		virtual int GetCategoryFlag() const override { return category; }

	class PFF_API Event {

	public:
		bool Handeled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlag() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) { return (GetCategoryFlag() & category); }

	};

	class EventDispacher {

		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispacher(Event& event) : m_Evemt(event) {}

		template<typename T>
		bool Dispach(EventFn<T> func) {

			if (m_Evemt.GetEventType() == T::GetStaticType()) {

				m_Evemt.Handeled = func(*(T*)&m_Evemt);
				return true;
			}
			return false;
		}

	private:

		Event& m_Evemt;
	};

	// inline std::ostream& operator<<(std::ostream & os, const Event & e) { return os << e.ToString(); }
}