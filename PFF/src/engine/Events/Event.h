#pragma once

#include "util/pffpch.h"


namespace PFF {

	// FIXME: Event are currently blocking => Fix that!

	enum class event_type {
		None = 0,
		WindowClose, WindowRefresh, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,			// window
		AppTick, AppUpdate, AppRender,																	// application
		KeyPressed, KeyReleased, MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled		// input
	};

	enum event_category {
		None				= 0,
		EC_Application		= BIT(0),
		EC_Input			= BIT(1),
		EC_Keyboard			= BIT(2),
		EC_Mouse			= BIT(3),
		EC_Button			= BIT(4)
	};

#define EVENT_CLASS_TYPE(type)					static event_type get_static_type() { return event_type::##type; }						\
												virtual event_type get_event_type() const override { return get_static_type(); }		\
												virtual const char* get_name() const override { return #type; }

#define	EVENT_CLASS_CATEGORY(category)			virtual int32 get_category_flag() const override { return category; }

#define EVENT_CLASS_STRING(custom_string)		std::string to_string() const override {												\
													std::stringstream ss;																\
													ss << "event - " << custom_string;													\
													return ss.str();}


	class PFF_API event {

	public:
		event() {}

		bool handled = false;

		virtual event_type get_event_type() const = 0;
		virtual const char* get_name() const = 0;
		virtual int32 get_category_flag() const = 0;
		virtual std::string to_string() const { return get_name(); }

		inline bool is_in_category(event_category category) { return (get_category_flag() & category); }

	};
	FORCEINLINE std::ostream& operator<<(std::ostream & os, const event & e) { return os << e.to_string(); }

	class PFF_API event_dispatcher {

		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		event_dispatcher(event& event) : m_event(event) {}

		template <typename T, typename Function>
		bool dispatch(Function func) {

			if (m_event.get_event_type() == T::get_static_type()) {

				m_event.handled = func(*(T*)&m_event);
				return true;
			}
			return false;
		}

	private:

		event& m_event;
	};

}
