#include "sorapch.h"
#include "ScriptGlue.h"

namespace Sora {

#define SORA_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Sora.InternalCalls::" #Name, Name)

    void ScriptGlue::RegisterFunctions()
    {

    }

}