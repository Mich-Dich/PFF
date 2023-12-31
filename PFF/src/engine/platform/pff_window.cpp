
// #include "glad/glad.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"

#include "pff_window.h"


namespace PFF {

	static bool s_GLFWinitialized = false;

	/*
	static void GLFW_error_callback(int errorCode, const char* description) {

		CORE_LOG(Error, "[GLFW Error: " << errorCode << "]: " << description);
	}*/

	pff_window::pff_window(std::string title, u32 width, u32 height)  {
		
		m_Data.title = title;
		m_Data.width = width;
		m_Data.height = height;
		m_Window = init_window();

		/*
		CORE_LOG(Trace, "Creating window [" << m_Data.Title << " width: " << m_Data.Width << "  height: " << m_Data.Height << "]");

		if (!s_GLFWinitialized) {

			CORE_ASSERT(glfwInit(), "GLFW initialized", "Could not initialize GLFW");

			glfwSetErrorCallback(GLFW_error_callback);
			s_GLFWinitialized = true;
		}

		m_Window = glfwCreateWindow((int)attributes.Width, (int)attributes.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		CORE_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "", "Failed to Initialize [glad]")
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {

			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);
			Data.Width = width;
			Data.Height = height;
			WindowResizeEvent event(width, height);
			Data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {

			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			Data.EventCallback(event);
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {

			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			Data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {

			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
			Data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {

					KeyPressedEvent event(key, 0);
					Data.EventCallback(event);
					break;
				}

				case GLFW_RELEASE: {

					KeyReleasedEvent event(key);
					Data.EventCallback(event);
					break;
				}

				case GLFW_REPEAT: {

					KeyPressedEvent event(key, 1);
					Data.EventCallback(event);
					break;
				}
				default:
					break;
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {

			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {

					MouseButtonPressedEvent event(button);
					Data.EventCallback(event);
					break;
				}

				case GLFW_RELEASE: {

					MouseButtonReleasedEvent event(button);
					Data.EventCallback(event);
					break;
				}

				default:
					break;
				}
		});
*/
	}

	pff_window::~pff_window() {

		LOG(Info, "Destroying pff_window");
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	GLFWwindow* pff_window::init_window() {

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		return glfwCreateWindow(m_Data.width, m_Data.height, m_Data.title.c_str(), nullptr, nullptr);
	}

	void pff_window::OnUpdate() {

		//glfwPollEvents();
		//glfwSwapBuffers(m_Window);
	}

	void pff_window::SetVSync(bool enable) {

		/*if (enable)	glfwSwapInterval(1);
		else			glfwSwapInterval(0);
		m_Data.VSync = enable;*/
	}

	bool pff_window::IsVSync() { return m_Data.VSync; }

	VkExtent2D pff_window::get_extend() { 
		
		return { static_cast<u32>(m_Data.width), static_cast<u32>(m_Data.height) }; 
	}

	bool pff_window::should_close() {
	
		return glfwWindowShouldClose(m_Window);
	}

	void pff_window::createWindowSurface(VkInstance_T* instance, VkSurfaceKHR_T** surface) {

		CORE_ASSERT(glfwCreateWindowSurface(instance, m_Window, nullptr, surface) == VK_SUCCESS, "", "Failed to create awindow surface");

	}

}
