#pragma once

#include <sstream>
#include <vector>

#include "engine/util.h"
#include "engine/types.h"
#include "Event.h"

namespace PFF {

	class PFF_API WindowResizeEvent : public Event {

	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: Width_m(width), Height_m(height) {}

		inline unsigned int GetWidth() const { return Width_m; }
		inline unsigned int GetHeight() const { return Height_m; }

		std::string ToString() const override {

			std::stringstream ss;
			ss << "Event - WindowResizeEvent [" << Width_m << ", " << Height_m << "]";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
			
	private:
		unsigned int Width_m, Height_m;
	};

	class PFF_API WindowCloseEvent : public Event {

	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class PFF_API AppTickEvent : public Event {

	public:
		AppTickEvent() {}
		
		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class PFF_API AppUpdateEvent : public Event {

	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class PFF_API AppRenderEvent : public Event {

	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}