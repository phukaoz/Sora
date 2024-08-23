#include "Yukipch.h"
#include "OrthographicCameraController.h"

#include "Yuki/Core/Input.h"
#include "Yuki/Core/Keycodes.h"

namespace Yuki {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool canRotate)
		:	m_AspectRatio(aspectRatio),
			m_CanRotate(canRotate),
			m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }),
			m_Camera(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		YUKI_PROFILE_FUNCTION();

		if (Input::IsKeyPressed(Key::A))
		{
			m_CameraPosition.x -= glm::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= glm::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		if (Input::IsKeyPressed(Key::D))
		{
			m_CameraPosition.x += glm::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += glm::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		if (Input::IsKeyPressed(Key::S))
		{
			m_CameraPosition.x -= -glm::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= glm::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		if (Input::IsKeyPressed(Key::W))
		{
			m_CameraPosition.x += -glm::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += glm::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}

		if (m_CanRotate)
		{
			if (Input::IsKeyPressed(Key::E))
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			if (Input::IsKeyPressed(Key::Q))
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

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		CalculateView();
	}

	void OrthographicCameraController::EnableRotation(bool canRotate)
	{
		YUKI_PROFILE_FUNCTION();

		m_CanRotate = canRotate;
	}

	void OrthographicCameraController::SetZoomLevel(float level)
	{
		m_ZoomLevel = level;
		CalculateView();
	}

	void OrthographicCameraController::CalculateView()
	{
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		YUKI_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		CalculateView();

		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		YUKI_PROFILE_FUNCTION();

		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		CalculateView();

		return false;
	}

}