#pragma once

#include "Yuki/Core/Window.h"
#include "Yuki/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Yuki {

	class WindowsWindow : public Window 
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline uint32_t GetWidth() const  override { return m_Data.Width; }
		inline uint32_t GetHeight() const override { return m_Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		inline void SetVSync(bool enabled) override;
		inline bool IsSync() const override;

		inline virtual void* GetNativeWindow() const override { return m_Window; }
	private:
		void Init(const WindowProps& props);
		void Shutdown();

		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData 
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}