#pragma once

#include "Yuki/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Yuki {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};

}