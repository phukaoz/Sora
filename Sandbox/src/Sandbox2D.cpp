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
	m_Tilemap = Sora::Texture2D::Create("assets/textures/tilemap.png");
	m_Crease = Sora::Texture2D::Create("assets/textures/crease.png");
	m_CheckerBoard = Sora::Texture2D::Create("assets/textures/checkerBoard.png");

	m_Sprite = Sora::SubTexture2D::Create(m_Tilemap, { 11, 2 }, { 8, 8 }, { 2, 2 });
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Sora::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

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

	Sora::Renderer2D::ResetStats();
	Sora::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Sora::RenderCommand::Clear();

	Sora::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Sora::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 20.0f, 20.0f }, m_CheckerBoard, 0.0f, 20.0f);
	for (float y = -5.0f; y <= 5.0f; y += 0.5f)
	{
		for (float x = -5.0f; x <= 5.0f; x += 0.5f)
		{
			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.3f, (y + 5.0f) / 10.0f, 1.0f };
			Sora::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
		}
	}
	Sora::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	SORA_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	auto stats = Sora::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCallCount);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::Text("FPS: %f", fps);
	ImGui::End();
}

void Sandbox2D::OnEvent(Sora::Event& e)
{
	m_CameraController.OnEvent(e);
}
