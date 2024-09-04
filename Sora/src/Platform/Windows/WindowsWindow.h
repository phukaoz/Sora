#pragma once

#include "Sora/Core/Window.h"
#include "Sora/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Sora {

	class WindowsWindow : public Window 
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline uint32_t GetWidth() const  override { return mData.Width; }
		inline uint32_t GetHeight() const override { return mData.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) override { mData.EventCallback = callback; }
		inline void SetVSync(bool enabled) override;
		inline bool IsSync() const override;

		inline virtual void* GetNativeWindow() const override { return mWindow; }
	private:
		void Init(const WindowProps& props);
		void Shutdown();

		GLFWwindow* mWindow;
		GraphicsContext* mContext;

		struct WindowData 
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData mData;
	};

}