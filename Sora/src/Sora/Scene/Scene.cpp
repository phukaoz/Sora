#include "sorapch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include "Sora/Scene/Entity.h"
#include "Sora/Scene/Component.h"
#include "Sora/Scene/ScriptableEntity.h"
#include "Sora/Renderer/Renderer2D.h"
#include "Sora/Scripting/ScriptEngine.h"

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

		template<typename... Component>
		static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
		{
			([&]() 
				{
                    auto view = src.view<Component>();
                    for (auto e : view)
                    {
                        UUID uuid = src.get<IDComponent>(e).ID;
                        entt::entity dstHandle = enttMap.at(uuid);

                        auto& srcComponent = src.get<Component>(e);
                        dst.emplace_or_replace<Component>(dstHandle, srcComponent);
                    }
				}(), ...);
		}

		template<typename...Component>
        static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
        {
			CopyComponent<Component...>(dst, src, enttMap);
        }

		template<typename... Component>
		static void CopyComponentIfExist(Entity dst, Entity src)
		{
			([&]()
				{
                    if (src.HasComponent<Component>())
                        dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
				}(), ...);
		}

        template<typename... Component>
        static void CopyComponentIfExist(ComponentGroup<Component...>, Entity dst, Entity src)
        {
            CopyComponentIfExist<Component...>(dst, src);
        }
	}

	Scene::Scene()
	{
		m_EditorCamera = EditorCamera(30.0f, 16.0f/9.0f, 0.1f, 1000.0f);
	}

	Scene::~Scene()
	{
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		if (!other)
			return newScene;

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;
		newScene->OnViewportResize(other->m_ViewportWidth, other->m_ViewportHeight);
		
		std::unordered_map<UUID, entt::entity> enttMap;
		auto& srcRegistry = other->m_Registry;
		auto& dstRegistry = newScene->m_Registry;
		auto view = srcRegistry.view<IDComponent>();
		for (auto e : view)
		{
			UUID uuid = srcRegistry.get<IDComponent>(e).ID;
			const std::string& name = srcRegistry.get<TagComponent>(e).Tag;
			Entity entity = newScene->CreateEntity(name, uuid);
			enttMap[uuid] = entity;
		}

		Utils::CopyComponent(AllComponents{}, dstRegistry, srcRegistry, enttMap);

		//newScene->mWorldID = other->mWorldID;
		
// 		auto& camera = newScene->GetEditorCamera();
// 		camera.SetDistance(other->GetEditorCamera().GetDistance());
// 		camera.SetFocalPoint(other->GetEditorCamera().GetFocalPoint());
// 		camera.SetPitch(other->GetEditorCamera().GetPitch());
// 		camera.SetYaw(other->GetEditorCamera().GetYaw());
		
		newScene->GetEditorCamera() = other->GetEditorCamera();
		
		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name, const std::optional<UUID>& uuid)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid.value_or(UUID()));
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Untitled Entity" : name;

		m_EntityMap[uuid.value_or(UUID())] = entity;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
		m_EntityMap.erase(entity.GetUUID());
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		Utils::CopyComponentIfExist(AllComponents{}, newEntity, entity);

		return newEntity;
	}

	void Scene::OnRuntimeStart()
	{
		OnPhysic2DStart();

		ScriptEngine::OnRuntimeStart(this);

		auto view = GetEnititiesWith<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };

			const auto& scriptComponent = entity.GetComponent<ScriptComponent>();
			if (ScriptEngine::EntityClassExists(scriptComponent.ClassName))
			{
				ScriptEngine::OnCreateEntity(entity);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		OnPhysic2DStop();
	
		ScriptEngine::OnRuntimeStop();
	}

    void Scene::OnSimulationStart()
    {
		OnPhysic2DStart();
    }

    void Scene::OnSimulationStop()
    {
		OnPhysic2DStop();
    }

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		OnRenderScene(camera);
	}

    void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
    {
		OnUpdatePhysics(ts);

		OnRenderScene(camera);
    }

    void Scene::OnUpdateRuntime(Timestep ts)
	{
		// Native Scripts
        auto viewNativeScript = GetEnititiesWith<NativeScriptComponent>();
        for (auto e : viewNativeScript)
        {
            Entity entity = { e, this };

            auto& nativeScriptComponent = entity.GetComponent<NativeScriptComponent>();
            if (!nativeScriptComponent.Instance)
            {
                nativeScriptComponent.Instance = nativeScriptComponent.InstantiateScript();
                if (nativeScriptComponent.Instance)
                {
                    nativeScriptComponent.Instance->m_Entity = Entity(entity, this);
                }
                nativeScriptComponent.Instance->OnCreate();
            }

            nativeScriptComponent.Instance->OnUpdate(ts);
        }

		// Scripts
		auto viewScript = GetEnititiesWith<ScriptComponent>();
		for (auto e : viewScript)
		{
			Entity entity = { e, this };

			auto& scriptComponent = entity.GetComponent<ScriptComponent>();
			if (ScriptEngine::EntityClassExists(scriptComponent.ClassName))
			{
				ScriptEngine::OnUpdateEntity(entity, ts);
			}
		}

		// Physics
		OnUpdatePhysics(ts);

		// Render
		Entity mainCamera = GetPrimaryCameraEntity();
		if (mainCamera)
		{
			auto& mainCameraCamera = mainCamera.GetComponent<CameraComponent>().Camera;
			auto mainCameraTransform = mainCamera.GetComponent<TransformComponent>().GetTransform();
			Renderer2D::BeginScene(mainCameraCamera, mainCameraTransform);

			auto viewTransformSprite = GetEnititiesWith<TransformComponent, SpriteRendererComponent>();
			for (auto entity : viewTransformSprite)
			{
				auto [transform, sprite] = viewTransformSprite.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}

			auto viewTransformCircle = GetEnititiesWith<TransformComponent, CircleRendererComponent>();
			for (auto entity : viewTransformCircle)
			{
				auto [transform, circle] = viewTransformCircle.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle, (int)entity);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		m_EditorCamera.SetViewportSize(width, height);

		auto viewCamera = GetEnititiesWith<CameraComponent>();
		for (auto entity : viewCamera)
		{
			auto& camera_component = viewCamera.get<CameraComponent>(entity);
			if (!camera_component.FixedAspectRatio)
				camera_component.Camera.SetViewportSize(width, height);
		}
	}

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto viewCamera = GetEnititiesWith<CameraComponent>();
        for (auto entity : viewCamera)
        {
            const auto& cameraComponent = viewCamera.get<CameraComponent>(entity);
            if (cameraComponent.Primary)
                return Entity(entity, this);
        }

        return {};
    }

    Entity Scene::GetEntityByUUID(UUID uuid)
    {
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
		{
			return { m_EntityMap.at(uuid), this };
		}

		return {};
    }

    void Scene::OnPhysic2DStart()
    {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = { 0.0f, -9.8f };
        m_WorldID = b2CreateWorld(&worldDef);

        auto viewRigidbody2D = GetEnititiesWith<Rigidbody2DComponent>();
        for (auto e : viewRigidbody2D)
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
			
            b2BodyId bodyID = b2CreateBody(m_WorldID, &bodyDef);
            // TODO: find the better way to store body id.
            memcpy(&rb2d.RuntimeBody, &bodyID, sizeof(b2BodyId));

            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

                b2ShapeDef shapeDef = b2DefaultShapeDef();

                shapeDef.density = bc2d.Density;
                shapeDef.friction = bc2d.Friction;
                shapeDef.restitution = bc2d.Restitution;

                b2Polygon polygon = b2MakeOffsetBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2{ bc2d.Offset.x, bc2d.Offset.y }, b2MakeRot(0.0f));
                b2ShapeId shapeID = b2CreatePolygonShape(bodyID, &shapeDef, &polygon);
                // TODO: find the better way to store shape id.
                memcpy(&bc2d.RuntimeFixture, &shapeID, sizeof(b2ShapeId));
            }

            else if (entity.HasComponent<CircleCollider2DComponent>())
            {
                auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.density = cc2d.Density;
                shapeDef.friction = cc2d.Friction;
                shapeDef.restitution = cc2d.Restitution;

                b2Circle circle;
                circle.center = { cc2d.Offset.x, cc2d.Offset.y };
                circle.radius = transform.Scale.x * cc2d.Radius;
                b2ShapeId shapeID = b2CreateCircleShape(bodyID, &shapeDef, &circle);
                // TODO: find the better way to store shape id.
                memcpy(&cc2d.RuntimeFixture, &shapeID, sizeof(b2ShapeId));
            }
        }
    }

    void Scene::OnPhysic2DStop()
    {
        b2DestroyWorld(m_WorldID);
    }

    void Scene::OnUpdatePhysics(Timestep ts)
    {
        const int32_t subStepCount = 4;
        b2World_Step(m_WorldID, ts, subStepCount);

        auto viewRigidbody2D = GetEnititiesWith<Rigidbody2DComponent>();
        for (auto e : viewRigidbody2D)
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

    void Scene::OnRenderScene(EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);

        auto groupTransformSprite = m_Registry.group<TransformComponent, SpriteRendererComponent>();
        for (auto entity : groupTransformSprite)
        {
            auto [transform, sprite] = groupTransformSprite.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
        }

        auto viewTransformCircle = GetEnititiesWith<TransformComponent, CircleRendererComponent>();
        for (auto entity : viewTransformCircle)
        {
            auto [transform, circle] = viewTransformCircle.get<TransformComponent, CircleRendererComponent>(entity);
            Renderer2D::DrawCircle(transform.GetTransform(), circle, (int)entity);
        }

        Renderer2D::EndScene();
    }

    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component)
    {
        static_assert(false);
    }

    template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
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
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if(m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

    template<>
    void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
    {
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

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}

}