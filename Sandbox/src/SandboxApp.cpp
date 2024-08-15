#include <Yuki.h>
#include <Yuki/Core/EntryPoint.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

class Sandbox : public Yuki::Application
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

Yuki::Application* Yuki::CreateApplication()
{
	return new Sandbox();
}