#pragma once

#include "util/util.h"

/*
#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"*/

struct GLFWwindow;

struct VkExtent2D;
struct VkInstance_T;
struct VkSurfaceKHR_T;


namespace PFF {


	struct WindowAttributes {

		std::string title;
		unsigned int width;
		unsigned int height;

		WindowAttributes(const std::string title = "Gluttony", unsigned int width = 1280, unsigned int height = 720)
			: title(title), width(width), height(height) {}
	};

	class pff_window {

	public:
		//using EventCallbackFn = std::function<void(Event&)>;

		pff_window(std::string title = "defaultWindow", u32 width = 300, u32 height = 200);
		~pff_window();

		DELETE_COPY(pff_window)

		void createWindowSurface(VkInstance_T* instance, VkSurfaceKHR_T** surface);

		VkExtent2D get_extend();
		bool should_close();
		

	private:
		GLFWwindow* init_window();
		void OnUpdate();
		void SetVSync(bool enable);
		bool IsVSync();


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
