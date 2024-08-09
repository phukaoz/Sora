#pragma once

#include "Yuki.h"

class Sandbox2D : public Yuki::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Yuki::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Yuki::Event& e) override;
private:
	Yuki::OrthographicCameraController m_CameraController;

	// Temporary
	Yuki::Ref<Yuki::VertexArray> m_SquareVA;
	Yuki::Ref<Yuki::Shader> m_FlatColorShader;
	Yuki::Ref<Yuki::Texture2D> m_Texture;

	struct ProfileResult
	{
		const char* Name;
		float Time;
	};

	std::vector<ProfileResult> m_ProfileResults;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};