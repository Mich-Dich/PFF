#pragma once

#include "util/util.h"

/*
#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"*/

	struct GLFWwindow;

namespace PFF {


	struct WindowAttributes {

		std::string title;
		unsigned int width;
		unsigned int height;

		WindowAttributes(const std::string title = "Gluttony", unsigned int width = 1280, unsigned int height = 720)
			: title(title), width(width), height(height) {}
	};

	class PFF_API pff_window {

	public:
		//using EventCallbackFn = std::function<void(Event&)>;

		pff_window(std::string title = "defaultWindow", u32 width = 300, u32 height = 200);
		~pff_window();

		pff_window(const pff_window&) = delete;
		pff_window& operator=(const pff_window&) = delete;

		GLFWwindow* init_window();
		void OnUpdate();
		void SetVSync(bool enable);
		bool IsVSync();

		inline bool should_close() { return glfwWindowShouldClose(m_Window); }

		inline u32 GetWidth() { return m_Data.width; }
		inline u32 GetHeight() { return m_Data.height; }
		//inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

		struct WindowData {

			std::string title;
			unsigned int width;
			unsigned int height;
			bool VSync;
			//EventCallbackFn EventCallback;
		};

		GLFWwindow* m_Window;
		WindowData m_Data;
	};


}