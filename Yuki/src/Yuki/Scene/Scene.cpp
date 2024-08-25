#include "Yukipch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include "Component.h"
#include "Entity.h"
#include "Yuki/Renderer/Renderer2D.h"

namespace Yuki {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Untitled Entity" : name;
		return entity;
	}

	void Scene::OnUpdate(Timestep ts)
	{
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& script = view.get<NativeScriptComponent>(entity);
				if (!script.Instance)
				{
					script.Instance = script.InstantiateScript();
					script.Instance->m_Entity = Entity(entity, this);
					script.Instance->OnCreate();
				}

				script.Instance->OnUpdate(ts);
			}
		}

		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = &transform.Transform;
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);

			auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuad(transform, sprite.Color);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

}