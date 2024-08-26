#pragma once

struct GLFWwindow;
struct VkExtent2D;
struct VkInstance_T;
struct VkSurfaceKHR_T;

class event;
class application;

namespace PFF {

	using EventCallbackFn = std::function<void(event&)>;

	enum class window_size_state {
		windowed,
		minimised,
		fullscreen,
		fullscreen_windowed,
	};

	struct window_attrib {

		std::string title;
		u32 pos_x = 100;
		u32 pos_y = 100;
		f64 cursor_pos_x{};
		f64 cursor_pos_y{};
		u32 width{};
		u32 height{};
		bool vsync{};
		application* app_ref{};
		window_size_state window_size_state = window_size_state::windowed;

		EventCallbackFn event_callback;

		window_attrib(const std::string title = "PFF - Editor", const u32 width = 1600, const  u32 height = 900, const  bool vsync = false, const EventCallbackFn& callback = nullptr)
			: title(title), width(width), height(height), vsync(vsync), event_callback(callback){}
	};

	// ==============================================================  window ==============================================================

	class pff_window {
	public:

		using EventCallbackFn = std::function<void(event&)>;

		pff_window(window_attrib attributes = window_attrib());
		~pff_window();

		PFF_DELETE_COPY_CONSTRUCTOR(pff_window);

		FORCEINLINE bool is_vsync() const { return m_data.vsync; }
		FORCEINLINE u32 get_width() const { return m_data.width; }
		FORCEINLINE u32 get_height() const { return m_data.height; }
		FORCEINLINE window_attrib get_attributes() const { return m_data; }
		FORCEINLINE GLFWwindow* get_window() const { return m_Window; }
		FORCEINLINE void set_event_callback(const EventCallbackFn& callback) { m_data.event_callback = callback; }
		FORCEINLINE f64 get_cursor_pos_x() const { return m_data.cursor_pos_x; }
		FORCEINLINE f64 get_cursor_pos_y() const { return m_data.cursor_pos_y; }
		FORCEINLINE window_size_state get_window_size_state() const { return m_data.window_size_state; }
		FORCEINLINE PFF_API_EDITOR void get_framebuffer_size(int* width, int* height);

		PFF_API void show_window(bool show);
		PFF_API bool is_maximized();
		PFF_API void minimize_window();
		PFF_API void restore_window();
		PFF_API void maximize_window();
		PFF_API void show_titlebar(bool show);
		PFF_API void get_monitor_size(int* width, int* height);

		void create_vulkan_surface(VkInstance_T* instance, VkSurfaceKHR_T** get_surface);

		VkExtent2D get_extend();
		bool should_close();
		void poll_events();
		PFF_API void capture_cursor();
		PFF_API void release_cursor();

		template<typename Func>
		void queue_event(Func&& func) {

			m_event_queue.push(func);
		}

	private:

		std::mutex m_event_queue_mutex;
		std::queue<std::function<void()>> m_event_queue;
		std::filesystem::path m_icon_path;

		void bind_event_calbacks();
		void set_vsync(bool enable);
	
		window_attrib m_data{};
		GLFWwindow* m_Window{};
	};

}
