#pragma once

#include "Sora/Core/Core.h"

#include "Sora/Core/Window.h"
#include "Sora/Core/LayerStack.h"
#include "Sora/Events/Event.h"
#include "Sora/Events/ApplicationEvent.h"

#include "Sora/Core/Timestep.h"

#include "Sora/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Sora {

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			SORA_CORE_ASSERT(index < Count, "");
			return Args[index];
		}
	};

	class Application
	{
	public:		
		Application(const std::string& name = "Sora App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();

		void OnEvent(Event& e);
		
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Close();

		ImGuiLayer* GetImGuiLayer() { return mImGuiLayer; }

		static inline Application& Get() { return *sInstance; }
		inline Window& GetWindow() { return *mWindow; }

		ApplicationCommandLineArgs GetCommandLineArgs() const { return mCommandLineArgs; }
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		ApplicationCommandLineArgs mCommandLineArgs;
		std::unique_ptr<Window> mWindow;
		ImGuiLayer* mImGuiLayer;
		bool mRunning = true;
		bool mMinimized = false;
		LayerStack mLayerStack;
		Timestep mTimestep;
		float mLastFrameTime = 0.0f;
	private:
		static Application* sInstance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);

}