#include <Yuki.h>

class TestLayer : public Yuki::Layer
{
public:
	TestLayer()
		: Layer("Test"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(glm::vec3(0.0f))
	{
		m_TriangleVA.reset(Yuki::VertexArray::Create());
		float triangleVertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f,
		};
		std::shared_ptr<Yuki::VertexBuffer> triangleVB;
		triangleVB.reset(Yuki::VertexBuffer::Create(triangleVertices, sizeof(triangleVertices)));
		triangleVB->SetLayout({
			{ Yuki::ShaderDataType::Float3, "a_Position" },
			{ Yuki::ShaderDataType::Float4, "a_Color" }
		});
		m_TriangleVA->AddVertexBuffer(triangleVB);
		uint32_t triangleIndices[3] = { 0, 1, 2 };
		std::shared_ptr<Yuki::IndexBuffer> triangleIB;
		triangleIB.reset(Yuki::IndexBuffer::Create(triangleIndices, 6));
		m_TriangleVA->SetIndexBuffer(triangleIB);

		m_SquareVA.reset(Yuki::VertexArray::Create());
		float squareVertices[4 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};
		std::shared_ptr<Yuki::VertexBuffer> squareVB;
		squareVB.reset(Yuki::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Yuki::ShaderDataType::Float3, "a_Position" },
		});
		m_SquareVA->AddVertexBuffer(squareVB);
		uint32_t sqaureIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Yuki::IndexBuffer> squareIB;
		squareIB.reset(Yuki::IndexBuffer::Create(sqaureIndices, 6));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string gradientShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position	= a_Position;
				v_Color		= a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}

		)";
		std::string gradientShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}

		)";
		m_GradientShader.reset(new Yuki::Shader(gradientShaderVertexSrc, gradientShaderFragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			
			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main()
			{
				v_Position	= a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}

		)";
		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}

		)";
		m_BlueShader.reset(new Yuki::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void OnUpdate(Yuki::Timestep ts) override
	{
		//YUKI_TRACE("Delta time: {0}s", ts.GetSeconds());

		if (Yuki::Input::IsKeyPressed(YUKI_KEY_A))
			m_CameraPosition.x -= m_CameraMovementSpeed * ts;
		if (Yuki::Input::IsKeyPressed(YUKI_KEY_D))
			m_CameraPosition.x += m_CameraMovementSpeed * ts;
		if (Yuki::Input::IsKeyPressed(YUKI_KEY_S))
			m_CameraPosition.y -= m_CameraMovementSpeed * ts;
		if (Yuki::Input::IsKeyPressed(YUKI_KEY_W))
			m_CameraPosition.y += m_CameraMovementSpeed * ts;
		if (Yuki::Input::IsKeyPressed(YUKI_KEY_E))
			m_CameraRotation -= m_CameraRotationSpeed * ts;
		if (Yuki::Input::IsKeyPressed(YUKI_KEY_Q))
			m_CameraRotation += m_CameraRotationSpeed * ts;

		Yuki::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Yuki::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Yuki::Renderer::BeginScene(m_Camera);

		Yuki::Renderer::Submit(m_BlueShader, m_SquareVA);
		Yuki::Renderer::Submit(m_GradientShader, m_TriangleVA);

		Yuki::Renderer::EndScene();

	}

	void OnEvent(Yuki::Event& event) override
	{
	}

private:
	std::shared_ptr<Yuki::Shader> m_GradientShader;
	std::shared_ptr<Yuki::VertexArray> m_TriangleVA;

	std::shared_ptr<Yuki::Shader> m_BlueShader;
	std::shared_ptr<Yuki::VertexArray> m_SquareVA;

	Yuki::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraRotation;
	float m_CameraMovementSpeed = 1.0f;
	float m_CameraRotationSpeed = 90.0f;
};

class Sandbox : public Yuki::Application
{
public:
	Sandbox()
	{
		PushLayer(new TestLayer());
	}

	~Sandbox()
	{
		
	}
};

Yuki::Application* Yuki::CreateApplication()
{
	return new Sandbox();
}