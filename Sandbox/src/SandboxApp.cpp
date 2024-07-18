#include <Yuki.h>

class TestLayer : public Yuki::Layer
{
public:
	TestLayer()
		: Layer("Test")
	{
	}

	void OnUpdate() override
	{
		if (Yuki::Input::IsKeyPressed(YUKI_KEY_TAB))
			YUKI_CORE_TRACE("Tab key is pressed!");
	}

	void OnEvent(Yuki::Event& event) override
	{

	}
};

class Sandbox : public Yuki::Application
{
public:
	Sandbox()
	{
		PushLayer(new TestLayer());
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