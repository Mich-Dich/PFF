#pragma once

#include <string>
#include <vector>
#include <functional>

#include "engine/Events/Event.h"
#include "engine/util.h"
#include "engine/types.h"

struct SDL_Window;

namespace PFF {

	struct WindowAttributes {

		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowAttributes(const std::string title = "PFFgame", unsigned int width = 1280, unsigned int height = 720)
			: Title(title), Width(width), Height(height) {}
	};

	class Window {

	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowAttributes& attributes);
		virtual ~Window();

		static Window* Create(const WindowAttributes& attributes = WindowAttributes());
		inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
		void OnUpdate();
		void Poll_for_Events();

		inline u32 GetWidth() const { return m_Data.Width; }
		inline u32 GetHeight() const { return m_Data.Height; }
		void SetVSync(bool enable);
		bool IsVSync() const;

	private:
		virtual void Init(const WindowAttributes& attributes);
		virtual void Shutdown();

		struct WindowData {

			std::string Title;
			u32 Width;
			u32 Height;
			bool VSync;
			EventCallbackFn EventCallback;
		};

		SDL_Window* m_Window;
		WindowData m_Data;
	};
}
