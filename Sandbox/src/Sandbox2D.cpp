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
	m_Tilemap = Yuki::Texture2D::Create("assets/textures/tilemap.png");
	m_Crease = Yuki::Texture2D::Create("assets/textures/crease.png");
	m_CheckerBoard = Yuki::Texture2D::Create("assets/textures/checkerBoard.png");

	m_Sprite = Yuki::SubTexture2D::Create(m_Tilemap, { 11, 2 }, { 8, 8 }, { 2, 2 });

	m_CameraController.SetZoomLevel(5.0f);
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Yuki::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	// FPS ////////////////////////////////
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
	////////////////////////////////////////

	////////////////////////////////////////
	Yuki::Renderer2D::ResetStats();
	Yuki::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Yuki::RenderCommand::Clear();
	////////////////////////////////////////

	Yuki::Renderer2D::BeginScene(m_CameraController.GetCamera());
	// Render //////////////////////////////
	Yuki::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 20.0f, 20.0f }, m_CheckerBoard, 0.0f, 20.0f);
	for (float y = -5.0f; y <= 5.0f; y += 0.5f)
	{
		for (float x = -5.0f; x <= 5.0f; x += 0.5f)
		{
			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.3f, (y + 5.0f) / 10.0f, 1.0f };
			Yuki::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
		}
	}
	////////////////////////////////////////
	Yuki::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	YUKI_PROFILE_FUNCTION();

	static bool dockspaceOpen = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit")) Yuki::Application::Get().Close();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Begin("Settings");
	auto stats = Yuki::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCallCount);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::Text("FPS: %f", fps);
	ImGui::End();

	ImGui::End();
}

void Sandbox2D::OnEvent(Yuki::Event& e)
{
	m_CameraController.OnEvent(e);
}
