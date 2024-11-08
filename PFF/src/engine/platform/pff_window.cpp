
#include "util/pffpch.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"
#include "application.h"

#include <stb_image.h>

#include "pff_window.h"


namespace PFF {

	static bool s_GLFWinitialized = false;
		
	static FORCEINLINE void GLFW_error_callback(int errorCode, const char* description) { CORE_LOG(Error, "[GLFW Error: " << errorCode << "]: " << description); }
	
	// =============================================================================  serializer  =============================================================================

	static void serialize_window_atributes(window_attrib* window_attributes, const PFF::serializer::option option) {

		if (option == serializer::option::save_to_file && window_attributes->window_size_state == window_size_state::minimised)
			window_attributes->window_size_state = window_size_state::windowed;

		serializer::yaml(config::get_filepath_from_configtype(util::get_executable_path(), config::file::editor), "window_attributes", option)
			.entry(KEY_VALUE(window_attributes->title))
			.entry(KEY_VALUE(window_attributes->pos_x))
			.entry(KEY_VALUE(window_attributes->pos_y))
			.entry(KEY_VALUE(window_attributes->width))
			.entry(KEY_VALUE(window_attributes->height))
			.entry(KEY_VALUE(window_attributes->vsync))
			.entry(KEY_VALUE(window_attributes->window_size_state));
	}

	// ================================================================================== setup ==================================================================================

	pff_window::pff_window(window_attrib attributes) :
		m_data(attributes) {

		PFF_PROFILE_FUNCTION();
		LOG_INIT();

		serialize_window_atributes(&m_data, serializer::option::load_from_file);
		
		if (!s_GLFWinitialized) {

			glfwSetErrorCallback(GLFW_error_callback);
			CORE_ASSERT(glfwInit(), "GLFW initialized", "Could not initialize GLFW");
			s_GLFWinitialized = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_MAXIMIZED, (m_data.window_size_state == window_size_state::fullscreen || m_data.window_size_state == window_size_state::fullscreen_windowed) ? GLFW_TRUE : GLFW_FALSE);

		int max_posible_height = 0;
		int max_posible_width = 0;
		int monitor_count;
		auto monitors = glfwGetMonitors(&monitor_count);

		for (int x = 0; x < monitor_count; x++) {

			int xpos, ypos, width, height;
			glfwGetMonitorWorkarea(monitors[x], &xpos, &ypos, &width, &height);
			max_posible_height = math::max(max_posible_height, height);
			max_posible_width = math::max(max_posible_width, width);

			CORE_LOG(Trace, "Monitor: " << x << " data: " << xpos << " / " << ypos << " / " << width << " / " << height);
		}

		// ensure window is never bigger than possible OR smaller then logical
		m_data.height = math::clamp((int)m_data.height, 200, max_posible_height);
		m_data.width = math::clamp((int)m_data.width, 300, max_posible_width);
		m_data.height -= 35;
		m_data.width -= 8;
		m_Window = glfwCreateWindow(static_cast<int>(m_data.width), static_cast<int>(m_data.height), m_data.title.c_str(), nullptr, nullptr);

		CORE_ASSERT(glfwVulkanSupported(), "", "GLFW does not support Vulkan");
		CORE_LOG(Trace, "Creating window [" << m_data.title << " width: " << m_data.width << "  height: " << m_data.height << "]");

		//glfwSetWindowPos(m_Window, 100, 100);
		glfwSetWindowPos(m_Window, m_data.pos_x, m_data.pos_y);
		CORE_LOG(Trace, "window pos [" << m_data.title << " X: " << m_data.pos_x << "  Y: " << m_data.pos_y << "]");
		
		glfwSetWindowUserPointer(m_Window, &m_data);
		glfwGetCursorPos(m_Window, &m_data.cursor_pos_x, &m_data.cursor_pos_y);
		set_vsync(m_data.vsync);

		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);
		
		//int monitor_count;
		//auto monitors = glfwGetMonitors(&monitor_count);

		//for (int x = 0; x < monitor_count; x++) {

		//	int xpos, ypos, width, height;
		//	glfwGetMonitorWorkarea(monitors[x], &xpos, &ypos, &width, &height);
		//	CORE_LOG(Trace, "Monitor: " << x << " data: "<< xpos << " / " << ypos << " / " << width << " / " << height);
		//}

		//   check vertical									  check horicontal
		//if ((m_data.pos_y + m_data.height > mode->height) || (m_data.pos_x + m_data.width > mode->width)) {
		//
		//	m_data.pos_y = std::max(m_data.pos_y - ((m_data.pos_y + m_data.height) - mode->height + 30), (u32)0);
		//	m_data.pos_x = std::max(m_data.pos_x - ((m_data.pos_x + m_data.width) - mode->width + 30), (u32)0);
		//}
		glfwSetWindowMonitor(m_Window, NULL, m_data.pos_x, m_data.pos_y, m_data.width, m_data.height, mode->refreshRate);

		if (m_data.window_size_state == window_size_state::fullscreen
			|| m_data.window_size_state == window_size_state::fullscreen_windowed)
			glfwMaximizeWindow(m_Window);

		// Set icon
		m_icon_path = PFF::util::get_executable_path() / "./assets/logo.png";
		GLFWimage icon;
		int channels;
		std::string iconPathStr = m_icon_path.string();
		icon.pixels = stbi_load(iconPathStr.c_str(), &icon.width, &icon.height, &channels, 4);
		glfwSetWindowIcon(m_Window, 1, &icon);
		stbi_image_free(icon.pixels);

		bind_event_calbacks();
	}

	pff_window::~pff_window() {

		PFF_PROFILE_FUNCTION();

		const bool is_maximized = this->is_maximized();
		int titlebar_vertical_offset = is_maximized ? 12 : 6;

		int loc_pos_x;
		int loc_pos_y;
		glfwGetWindowPos(m_Window, &loc_pos_x, &loc_pos_y);
		m_data.pos_x = loc_pos_x + 4;								// window padding
		m_data.pos_y = loc_pos_y + 25 + titlebar_vertical_offset;	// [custom titlebar height offset] + [aximize offset]

		serialize_window_atributes(&m_data, serializer::option::save_to_file);
		glfwDestroyWindow(m_Window);
		glfwTerminate();
		LOG_SHUTDOWN();
	}

	// ============================================================================== inplemantation ==============================================================================
	

	void pff_window::bind_event_calbacks() {

		PFF_PROFILE_FUNCTION();

		glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* window) {
			
			window_attrib& data = *(window_attrib*)glfwGetWindowUserPointer(window);
			window_refresh_event event;
			data.event_callback(event);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {

			window_attrib& Data = *(window_attrib*)glfwGetWindowUserPointer(window);

			if (Data.window_size_state == window_size_state::windowed) {

				Data.width = static_cast<u32>(width);
				Data.height = static_cast<u32>(height);
			}

			window_resize_event event(static_cast<u32>(width), static_cast<u32>(height));
			Data.event_callback(event);
		});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {

			window_attrib& data = *(window_attrib*)glfwGetWindowUserPointer(window);
			window_focus_event event(focused == GLFW_TRUE);
			data.event_callback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			
			window_attrib& Data = *(window_attrib*)glfwGetWindowUserPointer(window);
			window_close_event event;
			Data.event_callback(event);
		});

		glfwSetTitlebarHitTestCallback(m_Window, [](GLFWwindow* window, int x, int y, int* hit) {
			
			*hit = application::get().get_is_titlebar_hovered();
		});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y) {

			// CORE_LOG(Trace, "Moving Window");
		});

		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized) {

			window_attrib& Data = *(window_attrib*)glfwGetWindowUserPointer(window);
			Data.window_size_state = maximized? window_size_state::fullscreen : window_size_state::windowed;
			 CORE_LOG(Trace, "Maximize window: " << maximized);
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {

			window_attrib& Data = *(window_attrib*)glfwGetWindowUserPointer(window);
			mouse_event event_x(key_code::mouse_scrolled_x, static_cast<f32>(xOffset));
			mouse_event event_y(key_code::mouse_scrolled_y, static_cast<f32>(yOffset));
			Data.event_callback(event_x);
			Data.event_callback(event_y);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {

			window_attrib& Data = *(window_attrib*)glfwGetWindowUserPointer(window);
			mouse_event event_x(key_code::mouse_moved_x, static_cast<f32>(Data.cursor_pos_x - xPos));
			mouse_event event_y(key_code::mouse_moved_y, static_cast<f32>(Data.cursor_pos_y - yPos));
			Data.event_callback(event_x);
			Data.event_callback(event_y);

			Data.cursor_pos_x = xPos;
			Data.cursor_pos_y = yPos;
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

			window_attrib& Data = *(window_attrib*)glfwGetWindowUserPointer(window);
			key_event event(static_cast<key_code>(key), static_cast<key_state>(action));
			Data.event_callback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {

			window_attrib& Data = *(window_attrib*)glfwGetWindowUserPointer(window);
			key_event event(static_cast<key_code>(button), static_cast<key_state>(action));
			Data.event_callback(event);
		});

	}

	void pff_window::poll_events() {

		PFF_PROFILE_FUNCTION();

		glfwPollEvents();

		// prossess constom queue
		std::scoped_lock<std::mutex> lock(m_event_queue_mutex);
		while (m_event_queue.size() > 0) {

			auto& func = m_event_queue.front();
			func();
			m_event_queue.pop();
		}
	}

	void pff_window::capture_cursor() { glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }

	void pff_window::release_cursor() { glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

	void pff_window::set_vsync(bool enable) { CORE_LOG(Warn, "VSync functionality not supported yet"); }

	VkExtent2D pff_window::get_extend() { return { static_cast<u32>(m_data.width), static_cast<u32>(m_data.height) }; }

	bool pff_window::should_close() { return glfwWindowShouldClose(m_Window); }

	void pff_window::get_framebuffer_size(int* width, int* height) { glfwGetFramebufferSize(m_Window, width, height); }

	void pff_window::show_window(bool show) { show ? glfwShowWindow(m_Window) : glfwHideWindow(m_Window); }

	bool pff_window::is_maximized() { return static_cast<bool>(glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED)); }

	void pff_window::minimize_window() {
		
		LOG(Trace, "minimizing window");
		glfwIconifyWindow(m_Window);
		m_data.window_size_state = window_size_state::minimised;
		//application::set_render_state(system_state::inactive);
	}

	void pff_window::restore_window() {
		
		LOG(Trace, "restoring window");
		glfwRestoreWindow(m_Window); 
		//application::set_render_state(system_state::active);
	}

	void pff_window::maximize_window() { 
		
		LOG(Trace, "maximising window");
		glfwMaximizeWindow(m_Window);
		m_data.window_size_state = window_size_state::fullscreen_windowed;
		//application::set_render_state(system_state::active);
	}

	PFF_API void pff_window::show_titlebar(bool show) {
		
		glfwWindowHint(GLFW_TITLEBAR, show ? GLFW_TRUE : GLFW_FALSE);
	}

	void pff_window::get_monitor_size(int* width, int* height) {

		auto monitor = glfwGetWindowMonitor(m_Window);
		if (!monitor)
			monitor = glfwGetPrimaryMonitor();

		auto video_mode = glfwGetVideoMode(monitor);

		*width = video_mode->width;
		*height = video_mode->height;
	}


	void pff_window::create_vulkan_surface(VkInstance_T* instance, VkSurfaceKHR_T** get_surface) {

		CORE_ASSERT(glfwCreateWindowSurface(instance, m_Window, nullptr, get_surface) == VK_SUCCESS, "", "Failed to create awindow surface");
	}

}
