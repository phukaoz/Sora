#pragma once

#include "Yuki/Layer.h"

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
		virtual void OnImGuiRender() override;
		
		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}
