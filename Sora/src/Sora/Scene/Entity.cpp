#include "sorapch.h"
#include "Entity.h"

namespace Sora {

	Entity::Entity(entt::entity handle, Scene* scene)
		: mEntityHandle(handle), mScene(scene)
	{

	}

}