#include "sorapch.h"
#include "ScriptEngine.h"

#include "ScriptGlue.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>

namespace Sora {

    namespace Utils {

        static void PrintAssemblyTypes(MonoAssembly* assembly)
        {
            MonoImage* image = mono_assembly_get_image(assembly);
            const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
            const int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

            for (int32_t i = 0; i < numTypes; i++)
            {
                uint32_t cols[MONO_TYPEDEF_SIZE];
                mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

                const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
                const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

                SORA_CORE_TRACE("{}.{}", nameSpace, name);
            }
        }

        static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
        {
            std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

            SORA_CORE_ASSERT(stream, "Failed to open the file!");

            const std::streampos end = stream.tellg();
            stream.seekg(0, std::ios::beg);
            const uint32_t size = end - stream.tellg();

            if (size == 0)
            {
                SORA_CORE_WARN("File is empty!");
                return nullptr;
            }

            char* buffer = new char[size];
            stream.read((char*)buffer, size);
            stream.close();

            *outSize = size;
            return buffer;
        }

        static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
        {
            uint32_t fileSize = 0;
            char* fileData = Utils::ReadBytes(assemblyPath, &fileSize);

            // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
            MonoImageOpenStatus status;
            MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

            if (status != MONO_IMAGE_OK)
            {
                const char* errorMessage = mono_image_strerror(status);
                SORA_CORE_ERROR("Mono image error: {0}", errorMessage);
                return nullptr;
            }

            std::string path = assemblyPath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, path.c_str(), &status, 0);
            mono_image_close(image);

            // Don't forget to free the file data
            delete[] fileData;

            return assembly;
        }

    }

    struct ScriptEngineData
    {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;
        char AppDomainName[20] = "Sora-ScriptRuntime";

        MonoAssembly* CoreAssembly = nullptr;
        MonoImage* CoreAssemblyImage = nullptr;
    };

    static ScriptEngineData* s_Data = nullptr;

#pragma region ScriptEngine

    void ScriptEngine::Init()
    {
        s_Data = new ScriptEngineData;

        InitMono();
        LoadAssembly("resources/scripts/Sora-ScriptCore.dll");

        ScriptGlue::RegisterFunctions();
    }

    void ScriptEngine::Shutdown()
    {
        delete s_Data;
    }
    
    void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
    {
        s_Data->AppDomain = mono_domain_create_appdomain(s_Data->AppDomainName, nullptr);
        mono_domain_set(s_Data->AppDomain, true);

        s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
        s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
        // PrintAssemblyTypes(s_Data->CoreAssembly);
    }

    void ScriptEngine::InitMono()
    {
        mono_set_assemblies_path("mono/lib");

        MonoDomain* rootDomain = mono_jit_init("SoraJITRuntime");
        SORA_CORE_ASSERT(rootDomain, "Failed to init mono_jit!");

        s_Data->RootDomain = rootDomain;
    }

    void ScriptEngine::ShutdownMono()
    {
        s_Data->AppDomain = nullptr;
        s_Data->RootDomain = nullptr;
    }

    MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
    {
        MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
        mono_runtime_object_init(instance);

        return instance;
    }

#pragma endregion

#pragma region ScriptClass

    ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
        : m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
    }

    MonoObject* ScriptClass::Instantiate()
    {
        return ScriptEngine::InstantiateClass(m_MonoClass);
    }

    MonoMethod* ScriptClass::GetMethod(const std::string& name, int paramCount)
    {
        return mono_class_get_method_from_name(m_MonoClass, name.c_str(), paramCount);
    }

    MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
    {
        return mono_runtime_invoke(method, instance, params, nullptr);
    }

#pragma endregion

}