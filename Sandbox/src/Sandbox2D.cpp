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
	
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Yuki::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Yuki::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Yuki::RenderCommand::Clear();

	Yuki::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Yuki::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.6f, 0.6f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Yuki::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
	Yuki::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("setting");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Yuki::Event& e)
{
	m_CameraController.OnEvent(e);
}
