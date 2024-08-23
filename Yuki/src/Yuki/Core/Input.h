#pragma once

#include "Yuki/Core/Core.h"
#include "Yuki/Core/KeyCodes.h"
#include "Yuki/Core/MouseCodes.h"

namespace Yuki {
	
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);

		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}
