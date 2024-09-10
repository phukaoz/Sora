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
			SORA_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = mScene->mRegistry.emplace<T>(mEntityHandle, std::forward<Args>(args)...);
			mScene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = mScene->mRegistry.emplace_or_replace<T>(mEntityHandle, std::forward<Args>(args)...);
			mScene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			SORA_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");

			return mScene->mRegistry.get<T>(mEntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return mScene->mRegistry.all_of<T>(mEntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			SORA_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");

			mScene->mRegistry.remove<T>(mEntityHandle);
		}

		template<typename T, typename Func>
		void OnComponentAdded()
		{

		}

		operator bool() const { return mEntityHandle != entt::null; }
		operator entt::entity() const { return mEntityHandle; }
		operator uint32_t() const { return (uint32_t)mEntityHandle; }
		
		UUID				GetUUID()		{ return GetComponent<IDComponent>().ID; }
		const std::string&	GetName()		{ return GetComponent<TagComponent>().Tag; }
		glm::mat4			GetTransform()	{ return GetComponent<TransformComponent>().GetTransform(); }

		bool operator ==(const Entity& other) const
		{
			return mEntityHandle == other.mEntityHandle && mScene == other.mScene;
		}

		bool operator !=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity mEntityHandle = entt::null;
		Scene* mScene = nullptr;
	};

}