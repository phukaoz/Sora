#include <Yuki.h>

class Sandbox : public Yuki::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{
		
	}
};

Yuki::Application* Yuki::CreateApplication()
{
	return new Sandbox();
}