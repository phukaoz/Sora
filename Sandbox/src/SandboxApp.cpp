#include <Sora.h>
#include <Sora/Core/EntryPoint.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

class Sandbox : public Sora::Application
{
public:
	Sandbox()
	{
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
		
	}
};

Sora::Application* Sora::CreateApplication()
{
	return new Sandbox();
}