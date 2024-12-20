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
		Entity DuplicateEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
		void OnUpdateRuntime(Timestep ts);

		void OnViewportResize(uint32_t width, uint32_t height);

        EditorCamera& GetEditorCamera() { return m_EditorCamera; }

		Entity GetPrimaryCameraEntity();
		Entity GetEntityByUUID(UUID id);

		template<typename... Components>
		auto GetEnititiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		void OnPhysic2DStart();
		void OnPhysic2DStop();

        void OnUpdatePhysics(Timestep ts);
		void OnRenderScene(EditorCamera& camera);

        template<typename T>
        void OnComponentAdded(Entity entity, T& component);
	private:
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		b2WorldId m_WorldID = {};
		entt::registry m_Registry;

		EditorCamera m_EditorCamera;

		bool m_Saved = true;

		std::unordered_map<UUID, entt::entity> m_EntityMap;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

}