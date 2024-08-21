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

	Yuki::Ref<Yuki::Texture2D> m_Tilemap, m_Crease, m_CheckerBoard;
	Yuki::Ref<Yuki::SubTexture2D> m_Sprite;
};