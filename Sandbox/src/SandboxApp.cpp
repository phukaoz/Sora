#include <Yuki.h>
#include <Yuki/Core/EntryPoint.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

class TestLayer : public Yuki::Layer
{
public:
	TestLayer()
		: Layer("Test"), m_CameraController(1280.0f / 720.0f)
	{
		m_SquareVA = Yuki::VertexArray::Create();
		float squareVertices[4 * 6] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		Yuki::Ref<Yuki::VertexBuffer> squareVB = Yuki::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ Yuki::ShaderDataType::Float3, "a_Position" },
			{ Yuki::ShaderDataType::Float2, "a_TexCoord" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);
		uint32_t sqaureIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Yuki::Ref<Yuki::IndexBuffer> squareIB = Yuki::IndexBuffer::Create(sqaureIndices, 6);
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position	= a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}

		)";
		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}

		)";
		m_FlatColorShader = Yuki::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Yuki::Texture2D::Create("assets/textures/Brick.png");
		std::dynamic_pointer_cast<Yuki::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Yuki::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Yuki::Timestep ts) override
	{
		m_CameraController.OnUpdate(ts);

		Yuki::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Yuki::RenderCommand::Clear();

		Yuki::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Yuki::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Yuki::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = y%2; x < 20; x += 2)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Yuki::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		std::dynamic_pointer_cast<Yuki::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", glm::vec3(1.0f) - m_SquareColor);
		for (int y = 0; y < 20; y++)
		{
			for (int x = (y+1) % 2; x < 20; x += 2)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Yuki::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		glm::vec3 pos(-1.2f, 0.75f, 0.0f);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * 6.0f;
		Yuki::Renderer::Submit(textureShader, m_SquareVA, transform);

		Yuki::Renderer::EndScene();
	}

	void OnEvent(Yuki::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("setting");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

private:
	Yuki::ShaderLibrary m_ShaderLibrary;

	Yuki::Ref<Yuki::Shader> m_FlatColorShader;
	Yuki::Ref<Yuki::VertexArray> m_SquareVA;

	Yuki::Ref<Yuki::Texture2D> m_Texture;

	Yuki::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};

class Sandbox : public Yuki::Application
{
public:
	Sandbox()
	{
		//PushLayer(new TestLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
		
	}
};

Yuki::Application* Yuki::CreateApplication()
{
	return new Sandbox();
}