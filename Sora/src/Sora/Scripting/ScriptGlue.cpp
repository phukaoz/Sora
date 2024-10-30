#include "sorapch.h"
#include "ScriptGlue.h"

#include "ScriptEngine.h"
#include "Sora/Scene/Scene.h"
#include "Sora/Scene/Entity.h"

#include "Sora/Core/KeyCodes.h"
#include "Sora/Core/Input.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <box2d/box2d.h>

namespace Sora {

    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define SORA_ADD_INTERNAL_CALL(name) mono_add_internal_call("Sora.InternalCalls::" #name, name)

    namespace Utils {

        template<typename... Component>
        static void RegisterComponent()
        {
            ([]()
                {
                    std::string_view typeName = typeid(Component).name();
                    size_t pos = typeName.find_last_of(':');
                    std::string_view structName = typeName.substr(pos + 1);
                    std::string managedTypeName = fmt::format("Sora.{}", structName);

                    MonoType* managedType = mono_reflection_type_from_name(managedTypeName.data(), ScriptEngine::GetCoreAssemblyImage());
                    if (!managedType)
                    {
                        SORA_CORE_ERROR("Could not find component type {}", managedTypeName);
                        return;
                    }
                    s_EntityHasComponentFuncs[managedType] = [](Entity entity) {return entity.HasComponent<Component>(); };
                }(), ...);
        }

        template<typename... Component>
        static void RegisterComponent(ComponentGroup<Component...>)
        {
            RegisterComponent<Component...>();
        }

    }

#pragma region Entity

    static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        SORA_CORE_ASSERT(scene, "A scene is null!");
        Entity entity = scene->GetEntityByUUID(entityID);
        SORA_CORE_ASSERT(entity, "A entity is null!");

        MonoType* managedType = mono_reflection_type_get_type(componentType);
        SORA_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), "The entity has no this component!");
        return s_EntityHasComponentFuncs.at(managedType)(entity);
    }

#pragma endregion

#pragma region Transform Component

    static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        SORA_CORE_ASSERT(scene, "A scene is null!");
        Entity entity = scene->GetEntityByUUID(entityID);
        SORA_CORE_ASSERT(entity, "A entity is null!");

        *outTranslation = entity.GetComponent<TransformComponent>().Translation;
    }

    static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        SORA_CORE_ASSERT(scene, "A scene is null!");
        Entity entity = scene->GetEntityByUUID(entityID);
        SORA_CORE_ASSERT(entity, "A entity is null!");

        entity.GetComponent<TransformComponent>().Translation = *translation;
    }

#pragma endregion

#pragma region Rigidbody2D Component

    static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        SORA_CORE_ASSERT(scene, "A scene is null!");
        Entity entity = scene->GetEntityByUUID(entityID);
        SORA_CORE_ASSERT(entity, "A entity is null!");

        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
        b2BodyId body;
        memcpy(&body, &rb2d.RuntimeBody, sizeof(b2BodyId));

        b2Body_ApplyLinearImpulse(body, b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
    }

    static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        SORA_CORE_ASSERT(scene, "A scene is null!");
        Entity entity = scene->GetEntityByUUID(entityID);
        SORA_CORE_ASSERT(entity, "A entity is null!");

        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
        b2BodyId body;
        memcpy(&body, &rb2d.RuntimeBody, sizeof(b2BodyId));

        b2Body_ApplyLinearImpulseToCenter(body, b2Vec2(impulse->x, impulse->y), wake);
    }

#pragma endregion

#pragma region Input

    static bool Input_IsKeyPressed(KeyCode keycode)
    {
        return Input::IsKeyPressed(keycode);
    }

#pragma endregion

    void ScriptGlue::RegisterFunctions()
    {
        SORA_ADD_INTERNAL_CALL(Entity_HasComponent);

        SORA_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
        SORA_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

        SORA_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
        SORA_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

        SORA_ADD_INTERNAL_CALL(Input_IsKeyPressed);
    }

    void ScriptGlue::RegisterComponents()
    {
        Utils::RegisterComponent(AllComponents{});
    }

}