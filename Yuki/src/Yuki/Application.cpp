#include "Yukipch.h"
#include "Application.h"

#include "Yuki/Events/ApplicationEvent.h"
#include "Yuki/Log.h"

namespace Yuki {

	Application::Application() {
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application() {

	}

	void Application::Run() {

		while (m_Running) {
			m_Window->OnUpdate();
		}

	}

}