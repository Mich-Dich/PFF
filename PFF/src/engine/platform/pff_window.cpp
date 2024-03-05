
#include "util/pffpch.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"
#include "application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "util/io/serializer.h"

#include "pff_window.h"


namespace PFF {

	static bool s_GLFWinitialized = false;
		
	static void GLFW_error_callback(int errorCode, const char* description) {

		CORE_LOG(Error, "[GLFW Error: " << errorCode << "]: " << description);
	}

	// ================================================================================== setup ==================================================================================

	pff_window::pff_window(window_attrib attributes) :
		m_data(attributes) {

		PFF_PROFILE_FUNCTION();

		window_attr_serialiser(&m_data, "./config/window_attributes.txt").deserialize();

		init(m_data);
	}

	pff_window::~pff_window() {

		PFF_PROFILE_FUNCTION();

		int pos_x, pos_y;
		glfwGetWindowPos(m_Window, &pos_x, &pos_y);
		//config::save(config::file::editor, "window_attrib", "title", m_data.title);
		//config::save(config::file::editor, "window_attrib", "pos_x", pos_x);
		//config::save(config::file::editor, "window_attrib", "pos_y", pos_y);
		//config::save(config::file::editor, "window_attrib", "width", m_data.width);
		//config::save(config::file::editor, "window_attrib", "height", m_data.height);
		//config::save(config::file::editor, "window_attrib", "vsync", m_data.vsync);

		glfwDestroyWindow(m_Window);
		glfwTerminate();
		LOG(Info, "shutdown");
	}

	// ============================================================================== inplemantation ==============================================================================
	

	void pff_window::init(window_attrib attributes) {

		PFF_PROFILE_FUNCTION();

		//config::load(config::file::editor, "window_attrib", "title", attributes.title);
		//config::load(config::file::editor, "window_attrib", "pos_x", attributes.pos_x);
		//config::load(config::file::editor, "window_attrib", "pos_y", attributes.pos_y);
		//config::load(config::file::editor, "window_attrib", "width", attributes.width);
		//config::load(config::file::editor, "window_attrib", "height", attributes.height);
		//config::load(config::file::editor, "window_attrib", "vsync", attributes.vsync);
		attributes.app_ref = &application::get();
		m_data = attributes;

		if (!s_GLFWinitialized) {

			glfwSetErrorCallback(GLFW_error_callback);
			CORE_ASSERT(glfwInit(), "GLFW initialized", "Could not initialize GLFW");
			s_GLFWinitialized = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_TITLEBAR, false);

		m_Window = glfwCreateWindow(static_cast<int>(m_data.width), static_cast<int>(m_data.height), m_data.title.c_str(), nullptr, nullptr);
		CORE_ASSERT(glfwVulkanSupported(), "", "GLFW: Vulkan Not Supported");
		CORE_LOG(Trace, "Creating window [" << m_data.title << " width: " << m_data.width << "  height: " << m_data.height << "]");

		glfwSetWindowUserPointer(m_Window, &m_data);
		glfwGetCursorPos(m_Window, &m_data.cursor_pos_x, &m_data.cursor_pos_y);
		set_vsync(m_data.vsync);

		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);
		//glfwSetWindowMonitor(m_Window, NULL, m_data.pos_x, m_data.pos_y, m_data.width, m_data.height-30, mode->refreshRate);

		// Set icon
		GLFWimage icon;
		int channels;
		std::string iconPathStr = m_icon_path.string();
		icon.pixels = stbi_load(iconPathStr.c_str(), &icon.width, &icon.height, &channels, 4);
		glfwSetWindowIcon(m_Window, 1, &icon);
		stbi_image_free(icon.pixels);

		CORE_LOG(Info, "bind event callbacks");
		glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* window) {
			
			window_attrib& data = *(window_attrib*)glfwGetWindowUserPointer(window);
			window_refresh_event event;
			data.event_callback(event);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {

			window_attrib& Data = *(window_attrib*)glfwGetWindowUserPointer(window);
			Data.width = static_cast<u32>(width);
			Data.height = static_cast<u32>(height);
			window_resize_event event(Data.width, Data.height);
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

			window_attrib& Data = *(window_attrib*)glfwGetWindowUserPointer(window);
			*hit = Data.app_ref->is_titlebar_hovered();
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

		CORE_LOG(Info, "finished setup");
	}

	void pff_window::poll_events() {

		PFF_PROFILE_FUNCTION();

		glfwPollEvents();

		// prossess constom queue
		{
			std::scoped_lock<std::mutex> lock(m_event_queue_mutex);
			while (m_event_queue.size() > 0) {

				auto& func = m_event_queue.front();
				func();
				m_event_queue.pop();
			}
		}
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

	PFF_API_EDITOR void pff_window::get_framebuffer_size(int* width, int* height) {

		glfwGetFramebufferSize(m_Window, width, height);
	}

	void pff_window::minimize_window() {
		
		LOG(Fatal, "minimizing window");
		glfwIconifyWindow(m_Window);
		m_data.window_size_state = window_size_state::minimised;
		application::set_render_state(system_state::inactive);
	}

	void pff_window::restore_window() {
		
		LOG(Fatal, "restoring window");
		glfwRestoreWindow(m_Window); 
		m_data.window_size_state = window_size_state::windowed;
		application::set_render_state(system_state::active);
	}

	void pff_window::maximize_window() { 
		
		LOG(Fatal, "maximising window");
		glfwMaximizeWindow(m_Window);
		m_data.window_size_state = window_size_state::fullscreen_windowed;
		application::set_render_state(system_state::active);
	}

	bool pff_window::is_maximized() {

		return static_cast<bool>(glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED));
	}

	void pff_window::create_window_surface(VkInstance_T* instance, VkSurfaceKHR_T** get_surface) {

		CORE_ASSERT(glfwCreateWindowSurface(instance, m_Window, nullptr, get_surface) == VK_SUCCESS, "", "Failed to create awindow surface");

	}

	// =============================================================================  serializer  =============================================================================

	window_attr_serialiser::window_attr_serialiser(window_attrib* window_attributes, const std::string& filename)
		: m_window_attrib(window_attributes), m_filename(filename) {
	
	}

	window_attr_serialiser::~window_attr_serialiser() {}

	std::string window_attr_serialiser::window_size_state_to_string(window_size_state state) {

		switch (state) {
		case PFF::window_size_state::windowed:
			return "windowed";

		case PFF::window_size_state::minimised:
			return "minimised";

		case PFF::window_size_state::fullscreen:
			return "fullscreen";

		case PFF::window_size_state::fullscreen_windowed:
			return "fullscreen_windowed";

		default:
			break;
		}

		return "unknown";
	}

	window_size_state window_attr_serialiser::string_to_window_size_state(const std::string& string) {

		const std::unordered_map<std::string, window_size_state> state_map{
			{"windowed", window_size_state::windowed},
			{"minimised", window_size_state::minimised},
			{"fullscreen", window_size_state::fullscreen},
			{"fullscreen_windowed", window_size_state::fullscreen_windowed}
		};

		auto it = state_map.find(string);
		if (it != state_map.end())
			return it->second;
		
		return window_size_state::windowed; // Default value or any other error handling
	}

	//std::string				title;
	//u32						pos_x{};
	//u32						pos_y{};
	//f64						cursor_pos_x{};
	//f64						cursor_pos_y{};
	//u32						width{};
	//u32						height{};
	//bool						vsync{};
	//application*				app_ref{};
	//window_size_state			window_size_state = window_size_state::windowed;
	//EventCallbackFn			event_callback;


	void window_attr_serialiser::serialize() {

		serializer::yaml(m_filename, serializer::serializer_option::save_to_file)
			.struct_name("window_attributes")
			.set(KEY_VALUE(m_window_attrib->title))
			.set(KEY_VALUE(m_window_attrib->pos_x))
			.set(KEY_VALUE(m_window_attrib->pos_y))
			.set(KEY_VALUE(m_window_attrib->width))
			.set(KEY_VALUE(m_window_attrib->height))
			.set(KEY_VALUE(m_window_attrib->vsync))
			.serialize();
		
		LOG(Fatal, TEST_NAME_CONVERTION(window_size_state));
	}

	bool window_attr_serialiser::deserialize() {

		window_attrib test{};
		LOG(Trace, "test struct: title: " << test.title);

		serializer::yaml(m_filename, serializer::serializer_option::load_from_file)
			.deserialize()
			.struct_name("window_attributes")
			.get(KEY_VALUE(m_window_attrib->title))
			.get(KEY_VALUE(m_window_attrib->pos_x))
			.get(KEY_VALUE(m_window_attrib->pos_y))
			.get(KEY_VALUE(m_window_attrib->width))
			.get(KEY_VALUE(m_window_attrib->height))
			.get(KEY_VALUE(m_window_attrib->vsync));

		// serializer.deserialize_key_value(m_filename, serializer::serializer_option::load_from_file);

		return false;
	}

}
