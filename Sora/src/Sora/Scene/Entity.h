#pragma once

#include <entt.hpp>

#include "Sora/Scene/Scene.h"
#include "Sora/Scene/Component.h"
#include "Sora/Core/UUID.h"

namespace Sora {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			SORA_CORE_ASSERT(!HasComponent<T>(), "The entity already has this component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			SORA_CORE_ASSERT(HasComponent<T>(), "The entity has no this component!");

			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			SORA_CORE_ASSERT(HasComponent<T>(), "The entity has no this component!");

			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T, typename Func>
		void OnComponentAdded()
		{

		}

		operator bool()			const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t()		const { return (uint32_t)m_EntityHandle; }
		
		UUID				GetUUID()		{ return GetComponent<IDComponent>().ID; }
		const std::string&	GetName()		{ return GetComponent<TagComponent>().Tag; }
		glm::mat4			GetTransform()	{ return GetComponent<TransformComponent>().GetTransform(); }

		bool operator ==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator !=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_EntityHandle = entt::null;
		Scene* m_Scene = nullptr;
	};

}