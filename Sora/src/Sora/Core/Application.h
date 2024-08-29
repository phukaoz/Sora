#pragma once

#include "Core.h"

#include "Window.h"
#include "Sora/Core/LayerStack.h"
#include "Sora/Events/Event.h"
#include "Sora/Events/ApplicationEvent.h"

#include "Sora/Core/Timestep.h"

#include "Sora/ImGui/ImGuiLayer.h"

namespace Sora {

	class Application
	{
	public:		
		Application(const std::string& name = "Sora App");
		virtual ~Application();

		void Run();
		
		void OnEvent(Event& e);
		
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		Timestep m_Timestep;
		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}