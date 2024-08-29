#pragma once

#include "Sora/Core/Layer.h"

#include "Sora/Events/MouseEvent.h"
#include "Sora/Events/KeyEvent.h"
#include "Sora/Events/ApplicationEvent.h"

namespace Sora {
	
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Event& e) override;
		
		void Begin();
		void End();

		void EnableEvents(bool enable) { m_EventsEnabled = enable; }

		void SetDarkThemeColors();
	private:
		bool m_EventsEnabled = true;
		float m_Time = 0.0f;
	};

}
