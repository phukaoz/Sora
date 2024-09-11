#include "EditorLayer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Sora/Scene/SceneSerializer.h"
#include "Sora/Utils/PlatformUtils.h"
#include "Sora/Math/Math.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo.h>

namespace Sora {

	extern const std::filesystem::path gAssetPath;

	EditorLayer::EditorLayer()
		: Layer("Sandbox2D"), mCurrentScenePath()
	{
	}

	void EditorLayer::OnAttach()
	{
		mIconPlay = Texture2D::Create("resources/icons/Toolbar/PlayButton.png");
		mIconStop = Texture2D::Create("resources/icons/Toolbar/StopButton.png");

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width  = 1600;
		fbSpec.Height = 900;
		mFramebuffer = Framebuffer::Create(fbSpec);

		mActiveScene = CreateRef<Scene>();

		mEditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		mSceneHierarchyPanel.SetContext(mActiveScene);
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(Sora::Timestep ts)
	{
		if (FramebufferSpecification spec = mFramebuffer->GetSpecification();
			mViewportSize.x > 0.0f && mViewportSize.y > 0.0f &&
			(spec.Width != mViewportSize.x || spec.Height != mViewportSize.y))
		{
			mFramebuffer->Resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			mEditorCamera.SetViewportSize(mViewportSize.x, mViewportSize.y);
			mActiveScene->OnViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		}

		Renderer2D::ResetStats();
		mFramebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();
		mFramebuffer->ClearAttachment(1, -1);

		switch (mSceneState)
		{
		case SceneState::Edit:
			mEditorCamera.OnUpdate(ts);
			mActiveScene->OnUpdateEditor(ts, mEditorCamera);
			break;
		case SceneState::Play:
			mActiveScene->OnUpdateRuntime(ts);
			break;
		}

		auto mouse = ImGui::GetMousePos();
		mouse.x -= mViewportBounds[0].x;
		mouse.y -= mViewportBounds[0].y;
		glm::vec2 viewportSize = mViewportBounds[1] - mViewportBounds[0];
		mouse.y = viewportSize.y - mouse.y; // flip for OpenGL coordinate system.

		int mouseX = (int)mouse.x;
		int mouseY = (int)mouse.y;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = mFramebuffer->ReadPixel(1, mouseX, mouseY);
			mHoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, mActiveScene.get());
		}

		mFramebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		if (mViewportFocused)
		{
			ImGui::SetItemKeyOwner(ImGuiMod_Alt);
		}

		static bool dockspace_open = true;
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

		if (ImGui::Begin("DockSpace", &dockspace_open, windowFlags))
		{
			//ImGui::ShowDemoWindow();

			if (!optPadding)   ImGui::PopStyleVar();
			if (optFullscreen) ImGui::PopStyleVar(2);

			ImGuiIO&	io	  = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();

			float minWindowSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 370.0f;
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("Sora Dockspace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
			}
			style.WindowMinSize.x = minWindowSizeX;

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

				ImGui::EndMenuBar();
			}

			mSceneHierarchyPanel.OnImGuiRender();
			mContentBrowserPanel.OnImGuiRender();
			UI_Toolbar();
			UI_Viewport();
	
			ImGui::End();
		}

		
	}

	void EditorLayer::OnEvent(Event& e)
	{
		mEditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SORA_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(SORA_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);

		switch (e.GetKeyCode())
		{
		case Key::N:
			if (control && !shift) NewScene();
			break;
		case Key::O:
			if (control && !shift) OpenScene();
			break;
		case Key::S:
			if (control && shift) SaveSceneAs();
			else if (control && !shift) SaveScene();
			break;

		case Key::D:
			if (control && !shift) DuplicateEntity();
			break;

		case Key::Q:
			if (!control && !shift) mGizmoType = -1;
			break;
		case Key::W:
			if (!control && !shift) mGizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			if (!control && !shift) mGizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case Key::R:
			if (!control && !shift) mGizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (mViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				mSceneHierarchyPanel.SetSelectedEntity(mHoveredEntity);
		}

		return false;
	}

	void EditorLayer::NewScene()
	{
		mActiveScene = CreateRef<Scene>();
		mActiveScene->OnViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		mSceneHierarchyPanel.SetContext(mActiveScene);
		mCurrentScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Sora Scene (*.sora)\0*.sora\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (mSceneState == SceneState::Play)
			StopScene();

		Ref<Scene> newScene = CreateRef<Scene>(); 
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path))
		{
			mEditorScene = newScene;
			mEditorScene->OnViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			mSceneHierarchyPanel.SetContext(mEditorScene);
			
			mCurrentScenePath = path;

			mActiveScene = mEditorScene;
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Sora Scene (*.sora)\0*.sora\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(mActiveScene);
			serializer.Serialize(filepath);
			mCurrentScenePath = filepath;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (mCurrentScenePath.empty())
		{
			SaveSceneAs();
		}
		else
		{
			SceneSerializer serializer(mActiveScene);
			serializer.Serialize(mCurrentScenePath);
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

		if (ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			Ref<Texture2D> icon = mSceneState == SceneState::Edit ? mIconPlay : mIconStop;
			float size = ImGui::GetWindowHeight() - 4.0f;
			ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("play/stop", (ImTextureID)(uint64_t)icon->GetRendererID(), {size, size}))
			{
				if (mSceneState == SceneState::Edit)
					PlayScene();
				else if (mSceneState == SceneState::Play)
					StopScene();
			}

			ImGui::End();
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(3);
		}
	}

	void EditorLayer::UI_Viewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		if (ImGui::Begin("Viewport"))
		{
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			mViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			mViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			mViewportFocused = ImGui::IsWindowFocused();
			mViewportHovered = ImGui::IsWindowHovered();

			auto viewportPanelSize = ImGui::GetContentRegionAvail();
			mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			Application::Get().GetImGuiLayer()->EnableEvents(mViewportFocused || mViewportHovered);

			uint32_t textureID = mFramebuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)(uint64_t)textureID, ImVec2(mViewportSize.x, mViewportSize.y), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					OpenScene(gAssetPath / path);
				}

				ImGui::EndDragDropTarget();
			}

			// Gizmo's
			Entity selectedEntity = mSceneHierarchyPanel.GetSelectedEntity();
			if (selectedEntity && mGizmoType != -1)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				glm::mat4 cameraView = glm::mat4(1.0f);
				glm::mat4 cameraProjection = glm::mat4(1.0f);
				switch (mSceneState)
				{
				case SceneState::Edit:
					cameraProjection	= mEditorCamera.GetProjection();
					cameraView			= mEditorCamera.GetViewMatrix();
					break;
				case SceneState::Play :
					auto cameraEntity	= mActiveScene->GetPrimaryCameraEntity();
					const auto& camera	= cameraEntity.GetComponent<CameraComponent>().Camera;
					cameraProjection	= camera.GetProjection();
					cameraView			= glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
					break;
				}

				auto& transformComponent = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 transform = transformComponent.GetTransform();

				// edit snap value here!
				bool snap = Input::IsKeyPressed(Key::LeftControl);
				float snapValue = 0.5f;

				if (mGizmoType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)mGizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
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

	void EditorLayer::PlayScene()
	{
		mSceneState = SceneState::Play;
		mRuntimeScene = Scene::Copy(mEditorScene);
		mRuntimeScene->OnRuntimeStart();
		
		mActiveScene = mRuntimeScene;
	}

	void EditorLayer::StopScene()
	{
		mSceneState = SceneState::Edit;
		mRuntimeScene->OnRuntimeStop();
		mRuntimeScene = nullptr;

		mActiveScene = mEditorScene;
	}

	void EditorLayer::DuplicateEntity()
	{
		if (mSceneState != SceneState::Edit)
			return;

		Entity selectedEntity = mSceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
			mEditorScene->DuplicateEntity(selectedEntity);
	}

}