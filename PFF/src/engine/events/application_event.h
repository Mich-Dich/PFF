#pragma once

#include "event.h"


namespace PFF {

	class window_resize_event : public event {
	public:

		window_resize_event(u32 width, u32 height) : m_width(width), m_height(height) {}

		PFF_DEFAULT_GETTER_C(u32, width);
		PFF_DEFAULT_GETTER_C(u32, height);

		EVENT_CLASS_STRING("window resize event [" << m_width << ", " << m_height << "]");
		EVENT_CLASS_TYPE(WindowResize);
		EVENT_CLASS_CATEGORY(EC_Application);
			
	private:
		u32 m_width, m_height;
	};

	class window_focus_event : public event {
	public:

		window_focus_event(bool focus) : m_focus(focus) {}

		PFF_DEFAULT_GETTER_C(bool, focus);

		EVENT_CLASS_STRING("window focus event");
		EVENT_CLASS_TYPE(WindowFocus);
		EVENT_CLASS_CATEGORY(EC_Application);

	private:
		bool m_focus;
	};

	class window_close_event : public event {
	public:

		window_close_event() {}

		EVENT_CLASS_TYPE(WindowClose);
		EVENT_CLASS_CATEGORY(EC_Application);
	};

	class window_refresh_event : public event {
	public:

		window_refresh_event() {}

		EVENT_CLASS_TYPE(WindowRefresh);
		EVENT_CLASS_CATEGORY(EC_Application);
	};

	class app_Tick_event : public event {
	public:

		app_Tick_event() {}
		
		EVENT_CLASS_TYPE(AppTick);
		EVENT_CLASS_CATEGORY(EC_Application);
	};

	class app_update_event : public event {
	public:

		app_update_event() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EC_Application)
	};

	class app_render_event : public event {
	public:

		app_render_event() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EC_Application)
	};
}
