#pragma once

#include "Core.h"

namespace Yuki {

	class YUKI_API Application{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}