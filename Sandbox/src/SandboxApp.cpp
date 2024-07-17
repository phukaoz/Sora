#include <Yuki.h>

class Sandbox : public Yuki::Application
{
public:
	Sandbox()
	{
		PushOverlay(new Yuki::ImGuiLayer());
	}

	~Sandbox()
	{
		
	}
};

Yuki::Application* Yuki::CreateApplication()
{
	return new Sandbox();
}