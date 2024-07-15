#include "Yukipch.h"
#include "Application.h"

#include "Yuki/Events/ApplicationEvent.h"
#include "Yuki/Log.h"

namespace Yuki {

	Application::Application() {

	}

	Application::~Application() {

	}

	void Application::Run() {
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication)) {
			YUKI_INFO("is in category");
		} 

		while (true);
	}

}