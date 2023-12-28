#pragma once

#include "GLFW/glfw3.h"

#include "util/util.h"
#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"


namespace PFF {

	struct WindowAttributes {

		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowAttributes(const std::string title = "Gluttony", unsigned int width = 1280, unsigned int height = 720)
			: Title(title), Width(width), Height(height) {}
	};

	class PFF_API window {

	public:
		using EventCallbackFn = std::function<void(Event&)>;

		window(const WindowAttributes& attributes = WindowAttributes());
		~window();

		void OnUpdate();
		void SetVSync(bool enable);
		bool IsVSync();

		inline u32 GetWidth() { return m_Data.Width; }
		inline u32 GetHeight() { return m_Data.Height; }
		inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

		struct WindowData {

			std::string Title;
			unsigned int Width;
			unsigned int Height;
			bool VSync;
			EventCallbackFn EventCallback;
		};

		GLFWwindow* m_Window;
		WindowData m_Data;
	};


}