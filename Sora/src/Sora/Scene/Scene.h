#pragma once

#include <entt.hpp>
#include <box2d/box2d.h>

#include "Sora/Core/Timestep.h"
#include "Sora/Core/UUID.h"
#include "Sora/Renderer/EditorCamera.h"

namespace Sora {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();
		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string(), const std::optional<UUID>& uuid = std::nullopt);
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdatePhysics(Timestep ts);

		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateRuntime(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		uint32_t mViewportWidth = 0, mViewportHeight = 0;
		b2WorldId mWorldID;
		entt::registry mRegistry;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

}