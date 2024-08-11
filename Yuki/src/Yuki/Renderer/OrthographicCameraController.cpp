#include "Yukipch.h"
#include "OrthographicCameraController.h"

#include "Yuki/Core/Input.h"
#include "Yuki/Core/Keycodes.h"

namespace Yuki {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool canRotate)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_CanRotate(canRotate)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		YUKI_PROFILE_FUNCTION();

		if (Input::IsKeyPressed(YUKI_KEY_A))
		{
			m_CameraPosition.x -= glm::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= glm::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		if (Input::IsKeyPressed(YUKI_KEY_D))
		{
			m_CameraPosition.x += glm::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += glm::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		if (Input::IsKeyPressed(YUKI_KEY_S))
		{
			m_CameraPosition.x -= -glm::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= glm::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		if (Input::IsKeyPressed(YUKI_KEY_W))
		{
			m_CameraPosition.x += -glm::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += glm::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}

		if (m_CanRotate)
		{
			if (Input::IsKeyPressed(YUKI_KEY_E))
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			if (Input::IsKeyPressed(YUKI_KEY_Q))
				m_CameraRotation += m_CameraRotationSpeed * ts;
		
			m_Camera.SetRotation(m_CameraRotation);
		}
		
		m_CameraTranslationSpeed = m_ZoomLevel;
		m_Camera.SetPosition(m_CameraPosition);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		YUKI_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(YUKI_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(YUKI_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::EnableRotation(bool canRotate)
	{
		YUKI_PROFILE_FUNCTION();

		m_CanRotate = canRotate;
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		YUKI_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.2f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.1f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		YUKI_PROFILE_FUNCTION();

		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		return false;
	}

}