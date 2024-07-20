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
		if (event.GetEventType() == Yuki::EventType::KeyPressed)
		{
			auto& e = (Yuki::KeyPressedEvent&)event;
			YUKI_CORE_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
};

class Sandbox : public Yuki::Application
{
public:
	Sandbox()
	{
		PushLayer(new TestLayer());
	}

	~Sandbox()
	{
		
	}
};

Yuki::Application* Yuki::CreateApplication()
{
	return new Sandbox();
}