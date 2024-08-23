#include "Yukipch.h"
#include "Entity.h"

namespace Yuki {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{

	}

}