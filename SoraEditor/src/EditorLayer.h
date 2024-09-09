#pragma once

#include "Sora.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
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
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		void UI_Toolbar();
		void UI_Viewport();

		void PlayScene();
		void StopScene();
	private:
		Ref<Framebuffer> mFramebuffer;

		Ref<Scene> mActiveScene;
		Ref<Scene> mEditorScene, mRuntimeScene;
		std::filesystem::path mCurrentScenePath;

		EditorCamera mEditorCamera;
		bool mViewportFocused = false, mViewportHovered = false;
		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		glm::vec2 mViewportBounds[2];

		Entity mHoveredEntity;

		int mGizmoType = 0;

		enum class SceneState
		{
			Edit = 0, Play = 1,
		};

		SceneState mSceneState = SceneState::Edit;

		SceneHierarchyPanel mSceneHierarchyPanel;
		ContentBrowserPanel mContentBrowserPanel;

		Ref<Texture2D> mIconPlay, mIconStop;
	};

}