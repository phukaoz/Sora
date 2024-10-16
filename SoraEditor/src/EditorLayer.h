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

		void OnOverlayRender();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		// UI
		void UI_Toolbar();
		void UI_Viewport();
		void UI_DockspaceSetup();

        void OnScenePlay();
        void OnSceneSimulate();
		void OnSceneStop();
		
		void DuplicateEntity();
	private:
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene, m_RuntimeScene, m_SimulateScene;
		std::filesystem::path m_CurrentScenePath;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2] = { {}, {} };

		Entity m_HoveredEntity;

		int m_GizmoType = 0;

		bool m_ShowPhysicsColliders = false;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2,
		};

		SceneState m_SceneState = SceneState::Edit;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		Ref<Texture2D> m_IconPlay, m_IconStop, m_IconSimulate;
	};

}