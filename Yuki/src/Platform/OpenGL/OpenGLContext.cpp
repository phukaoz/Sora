#include "Yukipch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Yuki {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		YUKI_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		YUKI_CORE_ASSERT(status, "Failed to initialize Glad!");

		YUKI_CORE_INFO("");
		YUKI_CORE_INFO("+--------------------+");
		YUKI_CORE_INFO("| OpenGL Information |");
		YUKI_CORE_INFO("+--------------------+");
		YUKI_CORE_INFO("Vendor   : {0}", (const char*)glGetString(GL_VENDOR));
		YUKI_CORE_INFO("Renderer : {0}", (const char*)glGetString(GL_RENDERER));
		YUKI_CORE_INFO("Version  : {0}", (const char*)glGetString(GL_VERSION));
		YUKI_CORE_INFO("");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}
