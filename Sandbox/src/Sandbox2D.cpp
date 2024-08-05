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
	m_SquareVA = Yuki::VertexArray::Create();

	float squareVertices[4 * 3] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	Yuki::Ref<Yuki::VertexBuffer> squareVB;
	squareVB.reset(Yuki::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	squareVB->SetLayout({
		{ Yuki::ShaderDataType::Float3, "a_Position" }
	});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t sqaureIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Yuki::Ref<Yuki::IndexBuffer> squareIB;
	squareIB.reset(Yuki::IndexBuffer::Create(sqaureIndices, 6));
	m_SquareVA->SetIndexBuffer(squareIB);

	m_FlatColorShader = Yuki::Shader::Create("assets/shaders/FlatColor.glsl");
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Yuki::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Yuki::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Yuki::RenderCommand::Clear();

	Yuki::Renderer::BeginScene(m_CameraController.GetCamera());

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	std::dynamic_pointer_cast<Yuki::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Yuki::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);

	Yuki::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	Yuki::Renderer::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("setting");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Yuki::Event& e)
{
	m_CameraController.OnEvent(e);
}
