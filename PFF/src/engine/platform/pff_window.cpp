
// #include "glad/glad.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"

#include "pff_window.h"


namespace PFF {

	static bool s_GLFWinitialized = false;

	
	static void GLFW_error_callback(int errorCode, const char* description) {

		CORE_LOG(Error, "[GLFW Error: " << errorCode << "]: " << description);
	}

	// ================================================================================== setup ==================================================================================

	pff_window::pff_window(WindowAttributes attributes) :
		m_data(attributes) {
		
		init();
	}

	pff_window::~pff_window() {

		shutdown();
	}

	// ============================================================================== inplemantation ==============================================================================

	void pff_window::init() {

		if (!s_GLFWinitialized) {

			CORE_ASSERT(glfwInit(), "GLFW initialized", "Could not initialize GLFW");
			glfwSetErrorCallback(GLFW_error_callback);
			s_GLFWinitialized = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_Window = glfwCreateWindow(static_cast<int>(m_data.width), static_cast<int>(m_data.height), m_data.title.c_str(), nullptr, nullptr);

		CORE_LOG(Trace, "Creating window [" << m_data.title << " width: " << m_data.width << "  height: " << m_data.height << "]");

		//glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_data);
		SetVSync(true);

		glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* window) {
			
			WindowAttributes& data = *(WindowAttributes*)glfwGetWindowUserPointer(window);
			window_refresh_event event;
			data.EventCallback(event);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {

			WindowAttributes& Data = *(WindowAttributes*)glfwGetWindowUserPointer(window);
			Data.width = static_cast<u32>(width);
			Data.height = static_cast<u32>(height);
			window_resize_event event(Data.width, Data.height);
			Data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {

			WindowAttributes& Data = *(WindowAttributes*)glfwGetWindowUserPointer(window);
			window_close_event event;
			Data.EventCallback(event);
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {

			WindowAttributes& Data = *(WindowAttributes*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			Data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {

			WindowAttributes& Data = *(WindowAttributes*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
			Data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

			WindowAttributes& Data = *(WindowAttributes*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {

					key_pressed_event event(key, 0);
					Data.EventCallback(event);
				
				} break;

				case GLFW_RELEASE: {

					key_released_event event(key);
					Data.EventCallback(event);
				
				} break;

				case GLFW_REPEAT: {

					key_pressed_event event(key, 1);
					Data.EventCallback(event);
				
				} break;

				default:
					break;
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {

			WindowAttributes& Data = *(WindowAttributes*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS:{

					MouseButtonPressedEvent event(button);
					Data.EventCallback(event);
				
				}break;

				case GLFW_RELEASE: {

					MouseButtonReleasedEvent event(button);
					Data.EventCallback(event);
				
				} break;

				default:
					break;
			}
		});
	}

	void pff_window::shutdown() {

		LOG(Info, "Destroying pff_window");
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void pff_window::OnUpdate() {

		//glfwPollEvents();
		//glfwSwapBuffers(m_Window);
	}

	void pff_window::SetVSync(bool enable) {

		/*if (enable)	glfwSwapInterval(1);
		else			glfwSwapInterval(0);
		m_data.VSync = enable;*/
	}

	VkExtent2D pff_window::get_extend() { 
		
		return { static_cast<u32>(m_data.width), static_cast<u32>(m_data.height) };
	}

	bool pff_window::should_close() {
	
		return glfwWindowShouldClose(m_Window);
	}

	void pff_window::createWindowSurface(VkInstance_T* instance, VkSurfaceKHR_T** surface) {

		CORE_ASSERT(glfwCreateWindowSurface(instance, m_Window, nullptr, surface) == VK_SUCCESS, "", "Failed to create awindow surface");

	}

}
