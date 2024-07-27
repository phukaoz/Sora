#pragma once

#include "Core.h"

#include "Window.h"
#include "Yuki/LayerStack.h"
#include "Yuki/Events/Event.h"
#include "Yuki/Events/ApplicationEvent.h"

#include "Yuki/ImGui/ImGuiLayer.h"

#include "Yuki/Renderer/Shader.h"
#include "Yuki/Renderer/Buffer.h"
#include "Yuki/Renderer/VertexArray.h"

#include "Yuki/Renderer/OrthographicCamera.h"

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
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		
		std::shared_ptr<Shader> m_GradientShader;
		std::shared_ptr<VertexArray> m_TriangleVA;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;

		OrthographicCamera m_Camera;

		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}