#include "sorapch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Sora {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		SORA_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		SORA_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SORA_CORE_ASSERT(status, "Failed to initialize Glad!");

		SORA_CORE_INFO("");
		SORA_CORE_INFO("+--------------------+");
		SORA_CORE_INFO("| OpenGL Information |");
		SORA_CORE_INFO("+--------------------+");
		SORA_CORE_INFO("Vendor   : {0}", (const char*)glGetString(GL_VENDOR));
		SORA_CORE_INFO("Renderer : {0}", (const char*)glGetString(GL_RENDERER));
		SORA_CORE_INFO("Version  : {0}", (const char*)glGetString(GL_VERSION));
		SORA_CORE_INFO("");
	}

	void OpenGLContext::SwapBuffers()
	{
		SORA_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}

}
