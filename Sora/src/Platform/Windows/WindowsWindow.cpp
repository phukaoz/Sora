#include "sorapch.h"
#include "WindowsWindow.h"

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
		return CreateScope<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		SORA_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		SORA_PROFILE_FUNCTION();
		SORA_CORE_INFO("Shutdown window '{0}'", mData.Title);

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		SORA_PROFILE_FUNCTION();

		mData.Title = props.Title;
		mData.Width = props.Width;
		mData.Height = props.Height;

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
			
			mWindow = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
			
			int count = 0;
			int monitor_x = 0, monitor_y = 0;
			GLFWmonitor** monitor = glfwGetMonitors(&count);
			const GLFWvidmode* video_mode = glfwGetVideoMode(monitor[0]);
			glfwGetMonitorPos(monitor[0], &monitor_x, &monitor_y);
			
			glfwDefaultWindowHints();
			glfwSetWindowPos(mWindow, monitor_x + (video_mode->width - props.Width)/2, monitor_y + (video_mode->height - props.Height)/2);
			glfwShowWindow(mWindow);
		}

		mContext = new OpenGLContext(mWindow);
		mContext->Init();

		glfwSetWindowUserPointer(mWindow, &mData);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
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

		glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods) 
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

		glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});

	}

	void WindowsWindow::Shutdown()
	{
		SORA_PROFILE_FUNCTION();

		glfwDestroyWindow(mWindow);
	}

	void WindowsWindow::OnUpdate()
	{
		SORA_PROFILE_FUNCTION();

		glfwPollEvents();
		mContext->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		SORA_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		mData.VSync = enabled;
	}

	bool WindowsWindow::IsSync() const
	{
		return mData.VSync;
	}

}