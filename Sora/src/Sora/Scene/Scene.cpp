#include "sorapch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include "Component.h"
#include "Entity.h"
#include "Sora/Renderer/Renderer2D.h"

namespace Sora {

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

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		auto transfrom_sprite_group = m_Registry.group<TransformComponent, SpriteComponent>();
		for (auto entity : transfrom_sprite_group)
		{
			auto [transform, sprite] = transfrom_sprite_group.get<TransformComponent, SpriteComponent>(entity);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}
		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		auto native_script_view = m_Registry.view<NativeScriptComponent>();
		for (auto entity : native_script_view)
		{
			auto& script = native_script_view.get<NativeScriptComponent>(entity);
			if (!script.Instance)
			{
				script.Instance = script.InstantiateScript();
				if(script.Instance) script.Instance->m_Entity = Entity(entity, this);
				script.Instance->OnCreate();
			}

			script.Instance->OnUpdate(ts);
		}

		Camera* main_camera = nullptr;
		glm::mat4 camera_transform;
		auto transfrom_camera_view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : transfrom_camera_view)
		{
			auto [transform, camera] = transfrom_camera_view.get<TransformComponent, CameraComponent>(entity);
			if (camera.Primary)
			{
				main_camera = &camera.Camera;
				camera_transform = transform.GetTransform();
				break;
			}
		}

		if (main_camera)
		{
			Renderer2D::BeginScene(main_camera->GetProjection(), camera_transform);

			auto transfrom_sprite_group = m_Registry.group<TransformComponent, SpriteComponent>();
			for (auto entity : transfrom_sprite_group)
			{
				auto [transform, sprite] = transfrom_sprite_group.get<TransformComponent, SpriteComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		mViewportWidth = width;
		mViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& camera_component = view.get<CameraComponent>(entity);
			if (!camera_component.FixedAspectRatio)
				camera_component.Camera.SetViewportSize(width, height);
		}
	}

	Sora::Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera_component = view.get<CameraComponent>(entity);
			if (camera_component.Primary)
				return Entity(entity, this);
		}

		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<SpriteComponent>(Entity entity, SpriteComponent& component)
	{

	}
	
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if(mViewportWidth > 0 && mViewportHeight > 0)
			component.Camera.SetViewportSize(mViewportWidth, mViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}
}