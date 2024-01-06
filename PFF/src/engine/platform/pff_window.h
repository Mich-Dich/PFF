#pragma once


/*
#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"*/

struct GLFWwindow;
struct VkExtent2D;
struct VkInstance_T;
struct VkSurfaceKHR_T;

class event;


namespace PFF {

	using EventCallbackFn = std::function<void(event&)>;

	struct WindowAttributes {

		std::string title;
		u32 width;
		u32 height;
		bool VSync;
		EventCallbackFn EventCallback;

		WindowAttributes(const std::string title = "PFF - Sandbox", const u32 width = 1280, const  u32 height = 720, const  bool VSync = false, const EventCallbackFn& callback = nullptr)
			: title(title), width(width), height(height), VSync(VSync), EventCallback(callback){}
	};

	class pff_window {

	public:
		using EventCallbackFn = std::function<void(event&)>;

		pff_window(WindowAttributes);
		~pff_window();

		DELETE_COPY(pff_window);

		void createWindowSurface(VkInstance_T* instance, VkSurfaceKHR_T** get_surface);
		inline void SetEventCallback(const EventCallbackFn& callback) { m_data.EventCallback = callback; }

		VkExtent2D get_extend();
		bool should_close();
		FORCEINLINE bool IsVSync() const { return m_data.VSync; }
		FORCEINLINE u32 get_width() const { return m_data.width; }
		FORCEINLINE u32 get_height() const { return m_data.height; }
		FORCEINLINE WindowAttributes get_attributes() const { return m_data; }

		// TEMPORARY
		FORCEINLINE GLFWwindow* get_window() const { return m_Window; }
		
	private:
		
		void init();
		void shutdown();
		void OnUpdate();
		void SetVSync(bool enable);

		//EventCallbackFn EventCallback;
		WindowAttributes m_data;
		GLFWwindow* m_Window;
	};

}
