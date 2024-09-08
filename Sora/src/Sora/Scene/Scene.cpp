#include "sorapch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include "Component.h"
#include "Entity.h"
#include "Sora/Renderer/Renderer2D.h"

namespace Sora {

	namespace Utils{
		
		static b2BodyType TypeConvert(Rigidbody2DComponent::BodyType bodyType)
		{
			switch (bodyType)
			{
			case Rigidbody2DComponent::BodyType::Static:	return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:	return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic:	return b2_kinematicBody;
			}

			SORA_CORE_ASSERT(false, "Unknown body type!");
			return b2_staticBody;
		}

	}

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { mRegistry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Untitled Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		mRegistry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = { 0.0f, -10.0f };
		mWorldID = b2CreateWorld(&worldDef);

		auto view = mRegistry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = Utils::TypeConvert(rb2d.Type);
			bodyDef.position.x = transform.Translation.x;
			bodyDef.position.y = transform.Translation.y;
			bodyDef.rotation = b2MakeRot(transform.Rotation.z);
			bodyDef.fixedRotation = rb2d.FixedRotation;
			
			b2BodyId bodyID = b2CreateBody(mWorldID, &bodyDef);
			// TODO: find the better way to store body id.
			memcpy(&rb2d.RuntimeBody, &bodyID, sizeof(b2BodyId));
		
			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2ShapeDef shapeDef = b2DefaultShapeDef();
				shapeDef.density = bc2d.Density;
				shapeDef.friction = bc2d.Friction;
				shapeDef.restitution = bc2d.Restitution;

				b2Polygon polygon = b2MakeBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);
				b2ShapeId shapeID = b2CreatePolygonShape(bodyID, &shapeDef, &polygon);
				// TODO: find the better way to store shape id.
				memcpy(&bc2d.RuntimeFixture, &shapeID, sizeof(b2ShapeId));
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		b2DestroyWorld(mWorldID);
	}

	void Scene::OnUpdatePhysics(Timestep ts)
	{
		const int32_t subStepCount = 4;
		b2World_Step(mWorldID, ts, subStepCount);

		auto view = mRegistry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			// TODO: find the better way to get body.
			b2BodyId bodyID;
			memcpy(&bodyID, &rb2d.RuntimeBody, sizeof(b2BodyId));

			b2Vec2 position = b2Body_GetPosition(bodyID);
			transform.Translation.x = position.x;
			transform.Translation.y = position.y;

			b2Rot rotation = b2Body_GetRotation(bodyID);
			transform.Rotation.z = b2Rot_GetAngle(rotation);
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		auto transfrom_sprite_group = mRegistry.group<TransformComponent, SpriteComponent>();
		for (auto entity : transfrom_sprite_group)
		{
			auto [transform, sprite] = transfrom_sprite_group.get<TransformComponent, SpriteComponent>(entity);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}
		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		auto native_script_view = mRegistry.view<NativeScriptComponent>();
		for (auto entity : native_script_view)
		{
			auto& script = native_script_view.get<NativeScriptComponent>(entity);
			if (!script.Instance)
			{
				script.Instance = script.InstantiateScript();
				if(script.Instance) script.Instance->mEntity = Entity(entity, this);
				script.Instance->OnCreate();
			}

			script.Instance->OnUpdate(ts);
		}

		OnUpdatePhysics(ts);

		Camera* main_camera = nullptr;
		glm::mat4 camera_transform;
		auto transfrom_camera_view = mRegistry.view<TransformComponent, CameraComponent>();
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

			auto transfrom_sprite_group = mRegistry.group<TransformComponent, SpriteComponent>();
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

		auto view = mRegistry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& camera_component = view.get<CameraComponent>(entity);
			if (!camera_component.FixedAspectRatio)
				camera_component.Camera.SetViewportSize(width, height);
		}
	}

	Sora::Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = mRegistry.view<CameraComponent>();
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

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}

}