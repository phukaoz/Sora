#include "Sandbox2D.h"

#include "imgui/imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static float fps = 0.0f;

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
		static int frameCount = 0;
		static float fpsTimer = 0.0f;
		frameCount++;
		fpsTimer += ts;

		if (fpsTimer >= 1.0f)
		{
			fps = frameCount / fpsTimer;

			fpsTimer = 0.0f;
			frameCount = 0;
		}
	}

	Yuki::Renderer2D::ResetStats();
	{
		YUKI_PROFILE_SCOPE("Rederer Prep");
		Yuki::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Yuki::RenderCommand::Clear();
	}	
	
	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		YUKI_PROFILE_SCOPE("Renderer Draw");

		Yuki::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Yuki::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerBoard, 20.0f);
		Yuki::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 1.0f, 1.0f }, glm::radians(rotation), { 0.8f, 0.2f, 0.3f, 1.0f });
		Yuki::Renderer2D::DrawQuad({ 1.0f, 0.0f }, { 1.0f, 1.0f }, m_Brick);
		for (float y = -5.0f; y < 5.0f; y+=0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Yuki::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		Yuki::Renderer2D::EndScene();
	}

	if (Yuki::Input::IsMouseButtonPressed(YUKI_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Yuki::Input::GetMousePosition();
		auto width = Yuki::Application::Get().GetWindow().GetWidth();
		auto height = Yuki::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}
	
	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());
}

void Sandbox2D::OnImGuiRender()
{
	YUKI_PROFILE_FUNCTION();
	
	ImGui::Begin("Settings");
	
	auto stats = Yuki::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCallCount);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::Text("FPS: %f", fps);

	ImGui::End();
}

void Sandbox2D::OnEvent(Yuki::Event& e)
{
	m_CameraController.OnEvent(e);
}
