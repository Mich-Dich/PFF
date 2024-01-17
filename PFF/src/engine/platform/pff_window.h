#pragma once


struct GLFWwindow;
struct VkExtent2D;
struct VkInstance_T;
struct VkSurfaceKHR_T;

class event;


namespace PFF {

	using EventCallbackFn = std::function<void(event&)>;

	struct window_attributes {

		std::string title;
		u32 width;
		u32 height;
		f64 cursor_pos_x{};
		f64 cursor_pos_y{};
		bool vsync;
		EventCallbackFn event_callback;

		window_attributes(const std::string title = "PFF - Sandbox", const u32 width = 800, const  u32 height = 600, const  bool vsync = false, const EventCallbackFn& callback = nullptr)
			: title(title), width(width), height(height), vsync(vsync), event_callback(callback){}
	};

	class pff_window {

	public:
		using EventCallbackFn = std::function<void(event&)>;

		pff_window(window_attributes attributes = window_attributes());
		~pff_window();

		DELETE_COPY(pff_window);

		FORCEINLINE bool is_vsync() const { return m_data.vsync; }
		FORCEINLINE u32 get_width() const { return m_data.width; }
		FORCEINLINE u32 get_height() const { return m_data.height; }
		FORCEINLINE window_attributes get_attributes() const { return m_data; }
		FORCEINLINE GLFWwindow* get_window() const { return m_Window; }
		FORCEINLINE void set_event_callback(const EventCallbackFn& callback) { m_data.event_callback = callback; }
		FORCEINLINE f64 get_cursor_pos_x() const { return m_data.cursor_pos_x; }
		FORCEINLINE f64 get_cursor_pos_y() const { return m_data.cursor_pos_y; }

		void create_window_surface(VkInstance_T* instance, VkSurfaceKHR_T** get_surface);
		VkExtent2D get_extend();
		bool should_close();
		void update();
		
	private:
		
		void init(window_attributes attributes);
		void set_vsync(bool enable);

		window_attributes m_data;
		GLFWwindow* m_Window;
	};

}
