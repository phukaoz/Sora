#pragma once

#include "Yuki/Core/Layer.h"

#include "Yuki/Events/MouseEvent.h"
#include "Yuki/Events/KeyEvent.h"
#include "Yuki/Events/ApplicationEvent.h"

namespace Yuki {
	
	class YUKI_API ImGuiLayer : public Layer
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
