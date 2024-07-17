#pragma once

#include "Core.h"

#include "Window.h"
#include "Yuki/LayerStack.h"
#include "Yuki/Events/Event.h"
#include "Yuki/Events/ApplicationEvent.h"

#include "Window.h"

namespace Yuki {

	class YUKI_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		
		void OnEvent(Event& e);
		
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;

		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}