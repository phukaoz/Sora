#include "EditorLayer.h"

#include "imgui/imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Sora/Scene/SceneSerializer.h"
#include "Sora/Utils/PlatformUtils.h"
#include "Sora/Math/Math.h"

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

		FramebufferSpecification fb_spec;
		fb_spec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fb_spec.Width  = 1600;
		fb_spec.Height = 900;
		mFramebuffer = Framebuffer::Create(fb_spec);

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
		glm::vec2 viewport_size = mViewportBounds[1] - mViewportBounds[0];
		mouse.y = viewport_size.y - mouse.y; // flip for OpenGL coordinate system.

		int mouse_x = (int)mouse.x;
		int mouse_y = (int)mouse.y;

		if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < (int)viewport_size.x && mouse_y < (int)viewport_size.y)
		{
			int pixel_data = mFramebuffer->ReadPixel(1, mouse_x, mouse_y);
			mHoveredEntity = pixel_data == -1 ? Entity() : Entity((entt::entity)pixel_data, mActiveScene.get());
		}

		mFramebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockspace_open = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;
		if (!opt_padding) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		if (ImGui::Begin("DockSpace", &dockspace_open, window_flags))
		{
			if (!opt_padding)   ImGui::PopStyleVar();
			if (opt_fullscreen) ImGui::PopStyleVar(2);

			ImGuiIO&	io	  = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();

			float min_window_size_x = style.WindowMinSize.x;
			style.WindowMinSize.x = 370.0f;
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("Sora Dockspace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}
			style.WindowMinSize.x = min_window_size_x;

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

		switch (e.GetKeyCode())
		{
		case Key::N:
			if (control) NewScene();
			break;
		case Key::O:
			if (control) OpenScene();
			break;
		case Key::S:
			if (control && shift) SaveSceneAs();
			if (control && !shift) SaveScene();
			break;

		case Key::Q:
			mGizmoType = -1;
			break;
		case Key::W:
			mGizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			mGizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case Key::R:
			mGizmoType = ImGuizmo::OPERATION::SCALE;
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
		mCurrentScenePath = "";
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Sora Scene (*.yuki)\0*.yuki\0");
		if (!filepath.empty())
		{
			OpenScene(filepath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		mActiveScene = CreateRef<Scene>();
		mActiveScene->OnViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		mSceneHierarchyPanel.SetContext(mActiveScene);

		SceneSerializer serializer(mActiveScene);
		if (serializer.Deserialize(path))
		{
			mCurrentScenePath = path;
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Sora Scene (*.yuki)\0*.yuki\0");
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
		auto& hovered_color = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(hovered_color.x, hovered_color.y, hovered_color.z, 0.5f));
		auto& active_color = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(active_color.x, active_color.y, active_color.z, 0.5f));

		if (ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			Ref<Texture2D> icon = mSceneState == SceneState::Edit ? mIconPlay : mIconStop;
			float size = ImGui::GetWindowHeight() - 4.0f;
			ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("play/stop", (ImTextureID)icon->GetRendererID(), {size, size}))
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
			auto viewport_min_region = ImGui::GetWindowContentRegionMin();
			auto viewport_max_region = ImGui::GetWindowContentRegionMax();
			auto viewport_offset = ImGui::GetWindowPos();
			mViewportBounds[0] = { viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y };
			mViewportBounds[1] = { viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y };

			mViewportFocused = ImGui::IsWindowFocused();
			mViewportHovered = ImGui::IsWindowHovered();

			auto viewport_panel_size = ImGui::GetContentRegionAvail();
			mViewportSize = { viewport_panel_size.x, viewport_panel_size.y };

			Application::Get().GetImGuiLayer()->EnableEvents(mViewportFocused || mViewportHovered);

			uint32_t texture_id = mFramebuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)(uint64_t)texture_id, ImVec2(mViewportSize.x, mViewportSize.y), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

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
			Entity selected_entity = mSceneHierarchyPanel.GetSelectedEntity();
			if (selected_entity && mGizmoType != -1)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				float window_width = (float)ImGui::GetWindowWidth();
				float window_height = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, window_width, window_height);

				glm::mat4 camera_view = glm::mat4(1.0f);
				glm::mat4 camera_projection = glm::mat4(1.0f);
				switch (mSceneState)
				{
				case SceneState::Edit:
					camera_projection	= mEditorCamera.GetProjection();
					camera_view			= mEditorCamera.GetViewMatrix();
					break;
				case SceneState::Play :
					auto camera_entity	= mActiveScene->GetPrimaryCameraEntity();
					const auto& camera	= camera_entity.GetComponent<CameraComponent>().Camera;
					camera_projection	= camera.GetProjection();
					camera_view			= glm::inverse(camera_entity.GetComponent<TransformComponent>().GetTransform());
					break;
				}

				auto& transform_component = selected_entity.GetComponent<TransformComponent>();
				glm::mat4 transform = transform_component.GetTransform();

				// edit snap value here!
				bool snap = Input::IsKeyPressed(Key::LeftControl);
				float snap_value = 0.5f;

				if (mGizmoType == ImGuizmo::OPERATION::ROTATE)
					snap_value = 45.0f;

				float snap_values[3] = { snap_value, snap_value, snap_value };

				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					(ImGuizmo::OPERATION)mGizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
					nullptr, snap ? snap_values : nullptr);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					Math::DecomposeTransform(transform, translation, rotation, scale);

					transform_component.Translation = translation;
					transform_component.Rotation = rotation;
					transform_component.Scale = scale;
				}
			}

			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	void EditorLayer::PlayScene()
	{
		mSceneState = SceneState::Play;
		mActiveScene->OnRuntimeStart();
	}

	void EditorLayer::StopScene()
	{
		mSceneState = SceneState::Edit;
		mActiveScene->OnRuntimeStop();
	}
}