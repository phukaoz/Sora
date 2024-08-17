#pragma once

#include "Yuki.h"
#include "ParticleSystem.h"

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

	Yuki::Ref<Yuki::VertexArray> m_SquareVA;
	Yuki::Ref<Yuki::Shader> m_FlatColorShader;
	Yuki::Ref<Yuki::Texture2D> m_CheckerBoard, m_Brick;

	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;
};