#pragma once

#include "Yukipch.h"
#include "Yuki/Core/Core.h"
#include "Yuki/Events/Event.h"

namespace Yuki {
	
	struct WindowProps 
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Yuki Engine",
					uint32_t width = 1280,
					uint32_t height = 720)
			: Title(title), Width(width), Height(height) {}
	};

	class Window 
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}