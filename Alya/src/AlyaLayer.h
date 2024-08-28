#pragma once

#include "Yuki.h"
#include "Panels/SceneHierarchyPanel.h"

namespace Yuki {

	class AlyaLayer : public Yuki::Layer
	{
	public:
		AlyaLayer();
		virtual ~AlyaLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_Square1, m_Square2;
		Entity m_CameraEntity;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		SceneHierarchyPanel m_SceneHierarchyPanel;
	};

}