#include "sorapch.h"
#include "Win32Window.h"

#include "Sora/Events/ApplicationEvent.h"
#include "Sora/Events/KeyEvent.h"
#include "Sora/Events/MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Sora {

	static bool sGLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		SORA_CORE_ERROR("GLFW ERROR ({0}): {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<Win32Window>(props);
	}

	Win32Window::Win32Window(const WindowProps& props)
	{
		SORA_PROFILE_FUNCTION();

		Init(props);
	}

	Win32Window::~Win32Window()
	{
		SORA_PROFILE_FUNCTION();
		SORA_CORE_INFO("Shutdown window '{0}'", m_Data.Title);

		Shutdown();
	}

	void Win32Window::Init(const WindowProps& props)
	{
		SORA_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		SORA_CORE_INFO("Creating window '{0}' ({1}, {2})", props.Title, props.Width, props.Height);

		if (!sGLFWInitialized)
		{
			SORA_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			SORA_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			sGLFWInitialized = true;
		}

		{
			SORA_PROFILE_SCOPE("glfwCreateWindow");
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
			
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
			
			glfwMaximizeWindow(m_Window);
			glfwShowWindow(m_Window);
		}

		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});

	}

	void Win32Window::Shutdown()
	{
		SORA_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
	}

	void Win32Window::OnUpdate()
	{
		SORA_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void Win32Window::SetVSync(bool enabled)
	{
		SORA_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool Win32Window::IsSync() const
	{
		return m_Data.VSync;
	}

}