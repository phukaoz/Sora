#pragma once

#include "Yuki/Layer.h"

namespace Yuki {
	
	class YUKI_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);

	private:
		float m_Time;
	};

}
