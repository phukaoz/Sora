#include "EditorLayer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Sora/Scene/SceneSerializer.h"
#include "Sora/Utils/PlatformUtils.h"
#include "Sora/Math/Math.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo.h>

namespace Sora {

	const std::filesystem::path g_AssetPath = "assets/";

	EditorLayer::EditorLayer()
		: Layer("Sandbox2D"), m_CurrentScenePath()
	{
	}

	void EditorLayer::OnAttach()
	{
		m_IconPlay = Texture2D::Create("resources/icons/Toolbar/PlayButton.png");
		m_IconStop = Texture2D::Create("resources/icons/Toolbar/StopButton.png");

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width  = 1600;
		fbSpec.Height = 900;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = CreateRef<Scene>();

		m_SceneHierarchyPanel.SetContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(Sora::Timestep ts)
	{
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ActiveScene->GetEditorCamera().SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();
		m_Framebuffer->ClearAttachment(1, -1);

		switch (m_SceneState)
		{
		case SceneState::Edit:
			m_ActiveScene->GetEditorCamera().OnUpdate(ts);
			m_ActiveScene->OnUpdateEditor(ts, m_ActiveScene->GetEditorCamera());
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdateRuntime(ts);
			break;
		}

		auto mouse = ImGui::GetMousePos();
		mouse.x -= m_ViewportBounds[0].x;
		mouse.y -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		mouse.y = viewportSize.y - mouse.y; // flip for OpenGL coordinate system.

		int mouseX = (int)mouse.x;
		int mouseY = (int)mouse.y;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		OnOverlayRender();

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		//ImGui::ShowDemoWindow();
		static bool dockspaceOpen = true;
		static bool optFullscreen = true;
		static bool optPadding = false;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (optFullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}
		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_NoBackground;
		if (!optPadding) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		if (ImGui::Begin("DockSpace", &dockspaceOpen, windowFlags))
		{
			if (!optPadding)   ImGui::PopStyleVar();
			if (optFullscreen) ImGui::PopStyleVar(2);

			ImGuiIO&	io	  = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();

			float minWindowSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 370.0f;
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspaceID = ImGui::GetID("Sora Dockspace");
				ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
			}
			style.WindowMinSize.x = minWindowSizeX;

			UI_DockspaceSetup();

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New", "Ctrl+N"))				NewScene();
					if (ImGui::MenuItem("Open...", "Ctrl+O"))			OpenScene();
					if (ImGui::MenuItem("Save", "Ctrl+S"))				SaveScene();
					if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))	SaveSceneAs();
					if (ImGui::MenuItem("Exit"))						Sora::Application::Get().Close();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Tools"))
				{
					if (ImGui::MenuItem("Show/Hide Physics Colliders")) m_ShowPhysicsColliders ^= 1; // toggle

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			m_SceneHierarchyPanel.OnImGuiRender();
			m_ContentBrowserPanel.OnImGuiRender();
			UI_Toolbar();
			UI_Viewport();
	
			ImGui::End();
		}
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_ActiveScene->GetEditorCamera().OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SORA_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(SORA_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(KeyCode::LeftControl) || Input::IsKeyPressed(KeyCode::RightControl);
		bool shift = Input::IsKeyPressed(KeyCode::LeftShift) || Input::IsKeyPressed(KeyCode::RightShift);
		bool alt = Input::IsKeyPressed(KeyCode::LeftAlt) || Input::IsKeyPressed(KeyCode::RightAlt);

		switch (e.GetKeyCode())
		{
		case KeyCode::N:
			if (control && !shift) NewScene();
			break;
		case KeyCode::O:
			if (control && !shift) OpenScene();
			break;
		case KeyCode::S:
			if (control && shift) SaveSceneAs();
			else if (control && !shift) SaveScene();
			break;

		case KeyCode::D:
			if (control && !shift) DuplicateEntity();
			break;

		case KeyCode::Q:
			if (!control && !shift) m_GizmoType = -1;
			break;
		case KeyCode::W:
			if (!control && !shift) m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case KeyCode::E:
			if (!control && !shift) m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case KeyCode::R:
			if (!control && !shift) m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == MouseCode::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(KeyCode::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}

		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		switch (m_SceneState)
		{
			case EditorLayer::SceneState::Edit:
			{
				Renderer2D::BeginScene(m_ActiveScene->GetEditorCamera());

				break;
			}

			case EditorLayer::SceneState::Play:
			{
				Entity entity = m_ActiveScene->GetPrimaryCameraEntity();
				if (!entity)
					return;

				auto& camera = entity.GetComponent<CameraComponent>();
				auto& transform = entity.GetComponent<TransformComponent>();

				Renderer2D::BeginScene(camera.Camera, transform.GetTransform());

				break;
			}

			default:
			{
				return;
			}
		}

		if (m_ShowPhysicsColliders)
		{
			auto view = m_ActiveScene->GetEnititiesWith<TransformComponent, CircleCollider2DComponent>();
			for (auto entity : view)
			{
				auto [transformComponent, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

				glm::vec3 translation = transformComponent.Translation + glm::vec3(cc2d.Offset, 0.001f);
				glm::vec3 scale = transformComponent.Scale * glm::vec3(cc2d.Radius * 2.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) 
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawCircle(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.05f);
			}
		}

		if (m_ShowPhysicsColliders)
		{
			auto view = m_ActiveScene->GetEnititiesWith<TransformComponent, BoxCollider2DComponent>();
			for (auto entity : view)
			{
				auto [transformComponent, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

				glm::vec3 translation = transformComponent.Translation + glm::vec3(bc2d.Offset, 0.001f);
				glm::vec3 scale = transformComponent.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
					* glm::rotate(glm::mat4(1.0f), transformComponent.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawRect(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			}
		}

		Renderer2D::EndScene();
	}

	void EditorLayer::NewScene()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_EditorScene);
		m_CurrentScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Sora Scene (*.sora)\0*.sora\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		Ref<Scene> newScene = CreateRef<Scene>(); 
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);
			
			m_CurrentScenePath = path;

			m_ActiveScene = m_EditorScene;
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Sora Scene (*.sora)\0*.sora\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(filepath);
			m_CurrentScenePath = filepath;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (m_CurrentScenePath.empty())
		{
			SaveSceneAs();
		}
		else
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(m_CurrentScenePath);
		}
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		auto& colors = ImGui::GetStyle().Colors;
		auto& hoveredColor = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(hoveredColor.x, hoveredColor.y, hoveredColor.z, 0.5f));
		auto& activeColor = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(activeColor.x, activeColor.y, activeColor.z, 0.5f));

		ImGuiWindowFlags windowFlags = 0 
			| ImGuiWindowFlags_NoScrollWithMouse 
			| ImGuiWindowFlags_NoDecoration 
			| ImGuiWindowFlags_NoMove;
		ImGuiWindowClass windowClass;
		windowClass.DockNodeFlagsOverrideSet = 0 
			| ImGuiDockNodeFlags_NoTabBar 
			| ImGuiDockNodeFlags_NoUndocking;
		ImGui::SetNextWindowClass(&windowClass);

		if (ImGui::Begin("##Toolbar", nullptr, windowFlags))
		{
			Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
			float size = ImGui::GetWindowHeight() - 4.0f;
			ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("play/stop", (ImTextureID)(uint64_t)icon->GetRendererID(), {size, size}))
			{
				if (m_SceneState == SceneState::Edit)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}

			ImGui::End();
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(3);
		}
	}

	void EditorLayer::UI_Viewport()
	{
		ImGuiWindowFlags windowFlags = 0
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoMove;
		ImGuiWindowClass windowClass;
		windowClass.DockNodeFlagsOverrideSet = 0
			| ImGuiDockNodeFlags_NoTabBar
			| ImGuiDockNodeFlags_NoUndocking;
		ImGui::SetNextWindowClass(&windowClass);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		if (ImGui::Begin("Viewport", nullptr, windowFlags))
		{
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();

			auto viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			Application::Get().GetImGuiLayer()->EnableEvents(m_ViewportFocused || m_ViewportHovered);

			uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)(uint64_t)textureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					OpenScene(g_AssetPath / path);
				}

				ImGui::EndDragDropTarget();
			}

			// Gizmo's
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			if (selectedEntity && m_GizmoType != -1)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				glm::mat4 cameraView = glm::mat4(1.0f);
				glm::mat4 cameraProjection = glm::mat4(1.0f);
				switch (m_SceneState)
				{
				case SceneState::Edit:
					cameraProjection	= m_ActiveScene->GetEditorCamera().GetProjection();
					cameraView			= m_ActiveScene->GetEditorCamera().GetViewMatrix();
					break;
				case SceneState::Play :
					auto cameraEntity	= m_ActiveScene->GetPrimaryCameraEntity();
					const auto& camera	= cameraEntity.GetComponent<CameraComponent>().Camera;
					cameraProjection	= camera.GetProjection();
					cameraView			= glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
					break;
				}

				auto& transformComponent = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 transform = transformComponent.GetTransform();

				// edit snap value here!
				bool snap = Input::IsKeyPressed(KeyCode::LeftControl);
				float snapValue = 0.5f;

				if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
					nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					Math::DecomposeTransform(transform, translation, rotation, scale);

					transformComponent.Translation = translation;
					transformComponent.Rotation = rotation;
					transformComponent.Scale = scale;
				}
			}

			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	void EditorLayer::UI_DockspaceSetup()
	{
		static bool firstLoop = true;
		if (!firstLoop)
			return;

		ImGuiID dockspaceID = ImGui::GetID("Sora Dockspace");
		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace); 
		
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		uint32_t viewportWidth = viewport->Size.x;
		uint32_t viewportHeight = viewport->Size.y;
		ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

		/*
		*  Example
		* ┌──────────────────────────────────────────────────┐
		* │                        Top                       │
		* ├────────────┬────────────────────────┬────────────│
		* │            │                        │            │
		* │            │                        │            │
		* │    Left    │         Center         │    Right   │
		* │            │                        │            │
		* │            │                        │            │
		* │            │                        │            │
		* ├────────────┴────────────────────────┴────────────┤
		* │                                                  │
		* │                                                  │
		* │                      Bottom                      │
		* │                                                  │
		* └──────────────────────────────────────────────────┘
		*/

#define GetNodeSize(nodeID) ImGui::DockBuilderGetNode(nodeID)->Size
		ImGuiID DockMainID = dockspaceID;
		ImGuiID DockTopID, DockBottomID;
		ImGui::DockBuilderSplitNode(DockMainID, ImGuiDir_Up, 0.6f, &DockTopID, &DockBottomID);

		// Top part : split left right center
		ImGuiID DockLeftID, DockRightID, DockCenterID;
		ImGui::DockBuilderSplitNode(DockTopID, ImGuiDir_Up, 40.0f / GetNodeSize(DockTopID).y, &DockTopID, &DockCenterID);
		ImGui::DockBuilderSplitNode(DockCenterID, ImGuiDir_Left, 370.0f / GetNodeSize(DockCenterID).x, &DockLeftID, &DockCenterID);
		ImGui::DockBuilderSplitNode(DockCenterID, ImGuiDir_Right, 370.0f / GetNodeSize(DockCenterID).x, &DockRightID, &DockCenterID);
#undef GetNodeSize

		ImGui::DockBuilderDockWindow("Viewport", DockCenterID);
		ImGui::DockBuilderDockWindow("Scene Hierarchy", DockLeftID);
		ImGui::DockBuilderDockWindow("Properties", DockRightID);
		ImGui::DockBuilderDockWindow("Content Browser", DockBottomID);
		ImGui::DockBuilderDockWindow("##Toolbar", DockTopID);

		ImGui::DockBuilderFinish(dockspaceID);

		firstLoop = false;
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;
		m_RuntimeScene = Scene::Copy(m_EditorScene);
		m_RuntimeScene->OnRuntimeStart();
		
		m_ActiveScene = m_RuntimeScene;
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;
		m_RuntimeScene->OnRuntimeStop();
		m_RuntimeScene = nullptr;

		m_ActiveScene = m_EditorScene;
	}

	void EditorLayer::DuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
			m_EditorScene->DuplicateEntity(selectedEntity);
	}

}