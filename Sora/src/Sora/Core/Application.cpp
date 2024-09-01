#include "sorapch.h"
#include "Application.h"

#include "Sora/Renderer/Renderer.h"

#include "Input.h"

// Temporary
#include <GLFW/glfw3.h>

namespace Sora {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::sInstance = nullptr;

	Application::Application(const std::string& name /*= "Sora App"*/, ApplicationCommandLineArgs args /*= default*/)
		: mCommandLineArgs(args)
	{
		SORA_PROFILE_FUNCTION();

		SORA_CORE_ASSERT(!sInstance, "Application already exist");
		sInstance = this;

		mWindow = Window::Create(WindowProps(name));
		mWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		mImGuiLayer = new ImGuiLayer();
		PushOverlay(mImGuiLayer);
	}

	Application::~Application()
	{
		SORA_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		SORA_PROFILE_FUNCTION();

		mLayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		SORA_PROFILE_FUNCTION();

		mLayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		mRunning = false;
	}

	void Application::OnEvent(Event& e)
	{
		SORA_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		for (auto it = mLayerStack.end(); it != mLayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		SORA_PROFILE_FUNCTION();

		while (mRunning)
		{
			float time = (float)glfwGetTime(); // Platform::GetTime()
			Timestep timestep = time - mLastFrameTime;
			mLastFrameTime = time;

			if (!mMinimized)
			{
				{
					SORA_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : mLayerStack)
						layer->OnUpdate(timestep);
				}

				mImGuiLayer->Begin();
				{
					SORA_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : mLayerStack)
						layer->OnImGuiRender();
				}
				mImGuiLayer->End();

			}
			
			mWindow->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		SORA_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			mMinimized = true;
			return false;
		}
		mMinimized = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}