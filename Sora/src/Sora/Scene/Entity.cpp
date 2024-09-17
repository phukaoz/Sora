#include "sorapch.h"
#include "Entity.h"

namespace Sora {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{

	}

}