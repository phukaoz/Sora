#include "Sandbox2D.h"

#include "imgui/imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f/720.0f)
{
}

void Sandbox2D::OnAttach()
{
	YUKI_PROFILE_FUNCTION();

	m_CheckerBoard = Yuki::Texture2D::Create("assets/textures/checkerBoard.png");
	m_Brick = Yuki::Texture2D::Create("assets/textures/brick.png");
}

void Sandbox2D::OnDetach()
{
	YUKI_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Yuki::Timestep ts)
{
	YUKI_PROFILE_FUNCTION();

	m_CameraController.OnUpdate(ts);

	{
		YUKI_PROFILE_SCOPE("Rederer Prep");
		Yuki::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Yuki::RenderCommand::Clear();
	}	
	
	{
		YUKI_PROFILE_SCOPE("Renderer Draw");
		Yuki::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//Yuki::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f, 0.0f }, { 0.6f, 0.6f }, glm::radians(45.0f), m_Brick);
		//Yuki::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.6f, 0.6f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Yuki::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 2.0f, 2.0f }, m_Brick);
		Yuki::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { -1.0f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Yuki::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	YUKI_PROFILE_FUNCTION();

	ImGui::Begin("Performance Profiler");
	ImGui::End();
}

void Sandbox2D::OnEvent(Yuki::Event& e)
{
	m_CameraController.OnEvent(e);
}
