
#include "util/pffpch.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"
#include "application.h"

#include "pff_window.h"


namespace PFF {

	static bool s_GLFWinitialized = false;
		
	static void GLFW_error_callback(int errorCode, const char* description) {

		CORE_LOG(Error, "[GLFW Error: " << errorCode << "]: " << description);
	}

	// ================================================================================== setup ==================================================================================

	pff_window::pff_window(window_attributes attributes) :
		m_data(attributes) {

		PFF_PROFILE_FUNCTION();

		init(attributes);
	}

	pff_window::~pff_window() {

		PFF_PROFILE_FUNCTION();

		int pos_x, pos_y;
		glfwGetWindowPos(m_Window, &pos_x, &pos_y);
		config::save(config::file::editor, "window_attributes", "title", m_data.title);
		//config::save(config::file::editor, "window_attributes", "pos_x", pos_x);
		//config::save(config::file::editor, "window_attributes", "pos_y", pos_y);
		config::save(config::file::editor, "window_attributes", "width", m_data.width);
		config::save(config::file::editor, "window_attributes", "height", m_data.height);
		config::save(config::file::editor, "window_attributes", "vsync", m_data.vsync);

		glfwDestroyWindow(m_Window);
		glfwTerminate();
		LOG(Info, "shutdown");
	}

	// ============================================================================== inplemantation ==============================================================================
	

	void pff_window::init(window_attributes attributes) {

		PFF_PROFILE_FUNCTION();

		config::load(config::file::editor, "window_attributes", "title", attributes.title);
		//config::load(config::file::editor, "window_attributes", "pos_x", attributes.pos_x);
		//config::load(config::file::editor, "window_attributes", "pos_y", attributes.pos_y);
		config::load(config::file::editor, "window_attributes", "width", attributes.width);
		config::load(config::file::editor, "window_attributes", "height", attributes.height);
		config::load(config::file::editor, "window_attributes", "vsync", attributes.vsync);
		attributes.app_ref = &application::get();
		m_data = attributes;

		if (!s_GLFWinitialized) {

			CORE_ASSERT(glfwInit(), "GLFW initialized", "Could not initialize GLFW");
			glfwSetErrorCallback(GLFW_error_callback);
			s_GLFWinitialized = true;
		}

		glfwWindowHint(GLFW_TITLEBAR, false);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_Window = glfwCreateWindow(static_cast<int>(m_data.width), static_cast<int>(m_data.height), m_data.title.c_str(), nullptr, nullptr);
		CORE_ASSERT(glfwVulkanSupported(), "", "GLFW: Vulkan Not Supported");
		CORE_LOG(Trace, "Creating window [" << m_data.title << " width: " << m_data.width << "  height: " << m_data.height << "]");

		glfwSetWindowUserPointer(m_Window, &m_data);
		glfwGetCursorPos(m_Window, &m_data.cursor_pos_x, &m_data.cursor_pos_y);
		set_vsync(m_data.vsync);

		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);
		//glfwSetWindowMonitor(m_Window, NULL, m_data.pos_x, m_data.pos_y, m_data.width, m_data.height-30, mode->refreshRate);

		CORE_LOG(Info, "bind event callbacks");

		glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* window) {
			
			window_attributes& data = *(window_attributes*)glfwGetWindowUserPointer(window);
			window_refresh_event event;
			data.event_callback(event);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {

			window_attributes& Data = *(window_attributes*)glfwGetWindowUserPointer(window);
			Data.width = static_cast<u32>(width);
			Data.height = static_cast<u32>(height);
			window_resize_event event(Data.width, Data.height);
			Data.event_callback(event);
		});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {

			window_attributes& data = *(window_attributes*)glfwGetWindowUserPointer(window);
			window_focus_event event(focused == GLFW_TRUE);
			data.event_callback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			
			window_attributes& Data = *(window_attributes*)glfwGetWindowUserPointer(window);
			window_close_event event;
			Data.event_callback(event);
		});

		glfwSetTitlebarHitTestCallback(m_Window, [](GLFWwindow* window, int x, int y, int* hit) {

			window_attributes& Data = *(window_attributes*)glfwGetWindowUserPointer(window);
			*hit = Data.app_ref->is_titlebar_hovered();
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {

			window_attributes& Data = *(window_attributes*)glfwGetWindowUserPointer(window);
			mouse_event event_x(key_code::mouse_scrolled_x, static_cast<f32>(xOffset));
			mouse_event event_y(key_code::mouse_scrolled_y, static_cast<f32>(yOffset));
			Data.event_callback(event_x);
			Data.event_callback(event_y);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {

			window_attributes& Data = *(window_attributes*)glfwGetWindowUserPointer(window);
			mouse_event event_x(key_code::mouse_moved_x, static_cast<f32>(Data.cursor_pos_x - xPos));
			mouse_event event_y(key_code::mouse_moved_y, static_cast<f32>(Data.cursor_pos_y - yPos));
			Data.event_callback(event_x);
			Data.event_callback(event_y);

			Data.cursor_pos_x = xPos;
			Data.cursor_pos_y = yPos;
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

			window_attributes& Data = *(window_attributes*)glfwGetWindowUserPointer(window);
			key_event event(static_cast<key_code>(key), static_cast<key_state>(action));
			Data.event_callback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {

			window_attributes& Data = *(window_attributes*)glfwGetWindowUserPointer(window);
			key_event event(static_cast<key_code>(button), static_cast<key_state>(action));
			Data.event_callback(event);
		});

		CORE_LOG(Info, "finished setup");
	}

	void pff_window::poll_events() {

		PFF_PROFILE_FUNCTION();

		glfwPollEvents();
	}

	void pff_window::capture_cursor() {

		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void pff_window::release_cursor() {

		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void pff_window::set_vsync(bool enable) {

		CORE_LOG(Warn, "VSync functionality not supported yet");
	}


	VkExtent2D pff_window::get_extend() { 
		
		return { static_cast<u32>(m_data.width), static_cast<u32>(m_data.height) };
	}

	bool pff_window::should_close() {
	
		return glfwWindowShouldClose(m_Window);
	}

	void pff_window::create_window_surface(VkInstance_T* instance, VkSurfaceKHR_T** get_surface) {

		CORE_ASSERT(glfwCreateWindowSurface(instance, m_Window, nullptr, get_surface) == VK_SUCCESS, "", "Failed to create awindow surface");

	}

}
