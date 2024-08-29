#pragma once

#include "Sora/Core/Core.h"
#include "Sora/Core/KeyCodes.h"
#include "Sora/Core/MouseCodes.h"

namespace Sora {
	
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
