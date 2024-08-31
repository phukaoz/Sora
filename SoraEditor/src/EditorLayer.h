#pragma once

#include "Sora.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Sora/Renderer/EditorCamera.h"

namespace Sora {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts)	override;
		virtual void OnImGuiRender()		override;
		virtual void OnEvent(Event& e)		override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		Ref<Framebuffer> mFramebuffer;

		Ref<Scene> mActiveScene;

		EditorCamera mEditorCamera;
		bool mViewportFocused = false, mViewportHovered = false;
		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		glm::vec2 mViewportBounds[2];

		int mGizmoType = 0;

		SceneHierarchyPanel mSceneHierarchyPanel;

		Entity mHoveredEntity;
	};

}