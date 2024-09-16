#include "sorapch.h"
#include "EditorCamera.h"

#include "Sora/Core/Input.h"
#include "Sora/Core/KeyCodes.h"
#include "Sora/Core/MouseCodes.h"

#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Sora {

	EditorCamera::EditorCamera(float fov, float aspect_ratio, float near_clip, float far_clip)
		: m_FOV(fov), m_AspectRatio(aspect_ratio), m_NearClip(near_clip), m_FarClip(far_clip), Camera(glm::perspective(glm::radians(fov), aspect_ratio, near_clip, far_clip))
	{
		UpdateView();
	}

	void EditorCamera::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(KeyCode::LeftAlt))
		{
			const glm::vec2& mouse = { Input::GetMouseX(), Input::GetMouseY() };
			constexpr float speed = 0.003f;
			glm::vec2 delta = (mouse - m_InitialMousePosition) * speed;
			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(MouseCode::ButtonRight))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(MouseCode::ButtonLeft))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(MouseCode::ButtonMiddle))
				MouseZoom(delta.y);
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(SORA_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetFowardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView()
	{
		//mYaw = mPitch = 0.0f;
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		if (Input::IsKeyPressed(KeyCode::LeftAlt))
		{
			float delta = e.GetYOffset() * 0.1f;
			MouseZoom(delta);
			UpdateView();
		}

		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xspeed, yspeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xspeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * yspeed * m_Distance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yaw_sign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yaw_sign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetFowardDirection();
			m_Distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetFowardDirection() * m_Distance;
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f);
		float xfactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f);
		float yfactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return std::make_pair(xfactor, yfactor);
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		
		float speed = distance * distance;
		speed = std::min(speed, 100.0f);

		return speed;
	}

}