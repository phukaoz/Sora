#include "sorapch.h"
#include "ScriptGlue.h"

#include "ScriptEngine.h"
#include "Sora/Scene/Scene.h"
#include "Sora/Scene/Entity.h"

#include "Sora/Core/KeyCodes.h"
#include "Sora/Core/Input.h"

#include <mono/metadata/object.h>

namespace Sora {

#define SORA_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Sora.InternalCalls::" #Name, Name)

    static void NativeLog(MonoString* string, int param)
    {
        char* cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);

        std::cout << str << ", " << param << std::endl;
    }

    static void Entity_GetTranslation(UUID entityID, glm::vec3* outTranslation)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        *outTranslation = entity.GetComponent<TransformComponent>().Translation;
    }

    static void Entity_SetTranslation(UUID entityID, glm::vec3* translation)
    {
        Scene* scene = ScriptEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<TransformComponent>().Translation = *translation;
    }

    static bool Input_IsKeyPressed(KeyCode keycode)
    {
        return Input::IsKeyPressed(keycode);
    }

    void ScriptGlue::RegisterFunctions()
    {
        SORA_ADD_INTERNAL_CALL(NativeLog);

        SORA_ADD_INTERNAL_CALL(Entity_GetTranslation);
        SORA_ADD_INTERNAL_CALL(Entity_SetTranslation);

        SORA_ADD_INTERNAL_CALL(Input_IsKeyPressed);
    }

}