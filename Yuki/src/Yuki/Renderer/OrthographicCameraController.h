#pragma once

#include "Yuki/Renderer/OrthographicCamera.h"
#include "Yuki/Core/Timestep.h"

#include "Yuki/Events/ApplicationEvent.h"
#include "Yuki/Events/MouseEvent.h"

namespace Yuki {

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool canRotate = false);
		
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void EnableRotation(bool canRotate);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_CanRotate = false;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 1.0f, m_CameraRotationSpeed = 90.0f;
	};

}