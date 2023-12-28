
#include "glad/glad.h"

#include "engine/events/application_event.h"
#include "engine/events/mouse_event.h"
#include "engine/events/key_event.h"

#include "window.h"


namespace PFF {

	static bool s_GLFWinitialized = false;


	static void GLFW_error_callback(int errorCode, const char* description) {

		CORE_LOG(Error, "[GLFW Error: " << errorCode << "]: " << description);
	}

	window::window(const WindowAttributes& attributes) {

		m_Data.Title = attributes.Title;
		m_Data.Width = attributes.Width;
		m_Data.Height = attributes.Height;

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

	}

	window::~window() { 

		CORE_LOG(Trace, "Destroying window");
		glfwDestroyWindow(m_Window);
	}

	void window::OnUpdate() {

		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void window::SetVSync(bool enable) {

		if (enable)	glfwSwapInterval(1);
		else			glfwSwapInterval(0);
		m_Data.VSync = enable;
	}

	bool window::IsVSync() { return m_Data.VSync; }

}
